/*************************************************
  Copyright (C), 1988-1999, Zephyr Platform
  File name:      tplMap.h
  Author:         Zephyr Shannon
  Version:        V1.2
  Date:           12-22-2009
  Description:    Map template
  Others:         none
  Function List:  
      It looks like std::map, but more efficient,
      I take consider of the worst condition.
      And for the most important thing: U can read and debug it!
      滞后平衡算法：
      算法的核心思想是，不到逼不得已就不进行平衡，而且每次平衡只管自己的左右节点是否平衡，而不用管左右节点的子节点是否平衡。
      如果1边比另一边大一倍，就进行子树平衡，子树只保证当前节点的左右节点平衡，但是不必保证其子节点的平衡，算法可以在O(1)时间完成，这样在最差情况下
      每加一个节点，可以证明，如果左子树的子节点如果不平衡，那么有子树的子节点肯定会平衡，所以一次平衡，最差情况下最多会进行2Log(N)次平衡.所以整个算法的复杂度还是O(Log(N))
      而且某个节点平衡后，是又进入大致平衡状态的，所以即使在最差情况下，平衡的频率也会很低（可以证明，严格小于2/9）
      具体如果左子树大小是右子树的一倍，那么，如果还要向左子树插入节点，就回引起重平衡，但是如果向右节点加入节点，即使右节点加入之后，左子树还是大于右子树一倍，
      还是不会进行重平衡。在向右平衡时，取左子树的右子树大小，如果左子树的右子树小于右子树，那么就把左子树的右子树和右子树组成新的右子树（以根节点为新的右子树节点）
      如果大于，子取左子树的右子树的右子树(在取之前评断该操作是否会引起不平衡，如果会，则先平衡左子树左子树)，这步可能会引起左子树不平衡或者右子树不平衡。
  History:
*************************************************/
#ifndef _ZEPHYR_MAP_TEMPLATE_H_
#define _ZEPHYR_MAP_TEMPLATE_H_


#include "../../include/SysMacros.h"
#include "../../include/TypeDef.h"
#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif


#include "TplPool.h"

namespace Zephyr
{

template<class CItem,class CKey>
class TplNode : public CItem
{
public:
    //CKey                       GetKey();
    TplNode<CItem,CKey>*    m_pLeftNode;
    TplNode<CItem,CKey>*    m_pRightNode;
    TplNode<CItem,CKey>*    m_pParent;
#ifdef _DEBUG
    TUInt32               m_nodeSize:31;
    TUInt32               m_isActive:1;
#else
    TUInt32               m_nodeSize;
#endif
public:
    void     CheckTree()
    {
        if (m_pLeftNode)
        {
            m_pLeftNode->CheckTree();
        }
        CheckNode();
        if (m_pRightNode)
        {
            m_pRightNode->CheckTree();
        }
    }


private:
    enum EnActive
    {
        active_mask     = 0x80000000,
        deactive_mask   = 0x7FFFFFFF,
    };
    enum EnTreeSize
    {
        tree_size_mask  = 0x7FFFFFFF,
        balance_mask    = 0x80000000,
    };
    enum EnNextAddNode
    {
        left_node       = 0,
        right_node      = 1,
    };

    TplNode *GetSubRightTree(TUInt32 nSize,TplNode*& pSubTree);
    TplNode *GetSubLeftTree(TUInt32 nSize,TplNode *& pSubTree);
    TplNode *MigrateRight2Left()
    {
#ifdef _DEBUG
        if (m_pRightNode) //必须的，因为这个时候肯定是左子树大！bur
        {
#endif
            //要从左子树删右节点，不平衡，先平衡
            int nBalance = m_pRightNode->NeedBalance();
            if (nBalance>0)
            {
                m_pRightNode = m_pRightNode->MigrateLeft2Right();
            }
            else if (nBalance < 0)
            {
                m_pRightNode  = m_pRightNode->MigrateRight2Left();
            }
            //可以了,左子树平衡了
            //从左子树的右子树获取右子树.
            TplNode *pNewRoot;
            //如果左子树的右子树存在
            TplNode *pRLNode = m_pRightNode->m_pLeftNode;
            if (pRLNode)
            {
                //并且其大小小于1/2,
                if (pRLNode->m_nodeSize <= (m_pRightNode->m_nodeSize >>1))
                {
                    pNewRoot = m_pRightNode;
                    //直接删除整个左子树的右子树
                    m_pRightNode->m_pLeftNode = this;
                    m_pRightNode->m_pParent    = m_pParent;
                    m_pRightNode->m_nodeSize   = m_nodeSize;
                    if (m_pLeftNode)
                    {
                        m_nodeSize = m_pLeftNode->GetTreeSize();
                        m_nodeSize += pRLNode->GetTreeSize();
                    }
                    else
                    {
                        m_nodeSize = pRLNode->GetTreeSize();
                    }
                    m_pParent = m_pRightNode;
                    m_pRightNode = pRLNode;
                    pRLNode->m_pParent = this; //完了！
                    return pNewRoot;
                }
                else
                {
                    //平衡左子树的右子树的子节点
                    int nBalance = pRLNode->NeedBalance();
                    if (nBalance>0)
                    {
                        m_pRightNode->m_pLeftNode = pRLNode->MigrateLeft2Right();
                        pRLNode = m_pRightNode->m_pLeftNode;
                    }
                    else if (nBalance < 0)
                    {
                        m_pRightNode->m_pLeftNode = pRLNode->MigrateRight2Left();
                        pRLNode = m_pRightNode->m_pLeftNode;
                    }
                    //平衡了，使用左子树的右子树的右子树
                    TplNode *pRLLNode = pRLNode->m_pLeftNode;//->m_pRightNode; //肯定是存在的
                    if (pRLLNode)
                    {
                        pNewRoot = pRLNode;
                        m_pRightNode->m_pLeftNode = pRLNode->m_pRightNode;
                        if (pRLNode->m_pRightNode)
                        {
                            pRLNode->m_pRightNode->m_pParent = m_pRightNode;
                        }
                        m_pRightNode->m_nodeSize -= (pRLLNode->GetTreeSize()+1);
                        m_pRightNode->m_pParent = pRLNode;

                        pRLNode->m_pLeftNode = this;
                        pRLNode->m_pParent    = m_pParent;
                        pRLNode->m_nodeSize   = m_nodeSize;
                        pRLNode->m_pRightNode  = m_pRightNode;

                        pRLLNode->m_pParent = this;

                        m_pRightNode = pRLLNode;
                        if (m_pLeftNode)
                        {
                            m_nodeSize = m_pLeftNode->GetTreeSize();
                            m_nodeSize += pRLLNode->GetTreeSize();
                        }
                        else
                        {
                            m_nodeSize =pRLLNode->GetTreeSize();
                        }
                        m_pParent = pRLNode;
                        return pNewRoot;
                    }
                    else
                    {
                        pNewRoot = pRLNode;
                        m_pRightNode->m_pLeftNode = pRLNode->m_pRightNode;
                        if (pRLNode->m_pRightNode)
                        {
                            pRLNode->m_pRightNode->m_pParent = m_pRightNode;
                        }
                        --m_pRightNode->m_nodeSize ;
                        m_pRightNode->m_pParent = pRLNode;

                        pRLNode->m_pLeftNode = this;
                        pRLNode->m_pParent    = m_pParent;
                        pRLNode->m_nodeSize   = m_nodeSize;
                        pRLNode->m_pRightNode  = m_pRightNode;

                        //pRLLNode->m_pParent = this;

                        m_pRightNode = NULL;
                        if (m_pLeftNode)
                        {
                            m_nodeSize = m_pLeftNode->GetTreeSize();
                        }
                        else
                        {
                            m_nodeSize = 0;
                        }
                        m_pParent = pRLNode;
                        return pNewRoot;
                    }
                    //以左子树的右子树的右子树,为迁出节点
                }
            }
            else //if ((rightNodeSize) > ((leftNodeSize+1)<<1)) "+1"或者是叶节点 引起的
            {
                //直接平衡,把右子树提上来当root
                pNewRoot = m_pRightNode;
                m_pRightNode->m_pLeftNode = this;
                m_pRightNode->m_pParent    = m_pParent;
                m_pRightNode->m_nodeSize   = m_nodeSize;
                if (m_pLeftNode)
                {
                    m_nodeSize = m_pLeftNode->GetTreeSize();
                }
                else
                {
                    m_nodeSize = 0;
                }
                m_pParent = m_pRightNode;
                m_pRightNode = NULL;
                return pNewRoot;
            }
            //得到了.
#ifdef _DEBUG
        }
        return NULL;
#endif
    }
    //将nSize个节点从左子树迁移到右子树，返回的是新的根,这个迁移之后树并不一定平衡
    //但是我们可以肯定，其子树最差情况是1:3，所这个时候，
    //再对不平衡子树进行平衡后，最差情况(递归子平衡后，3会分出的情况为1-4/3（==(2/3)*(2/3)）)，变成7:5
    TplNode *MigrateLeft2Right() 
    {
#ifdef _DEBUG
        if (m_pLeftNode) //必须的，因为这个时候肯定是左子树大！bur
        {
#endif
            //要从左子树删右节点，不平衡，先平衡
            int nBalance = m_pLeftNode->NeedBalance();
            if (nBalance>0)
            {
                m_pLeftNode = m_pLeftNode->MigrateLeft2Right();
            }
            else if (nBalance < 0)
            {
                m_pLeftNode  = m_pLeftNode->MigrateRight2Left();
            }
            //可以了,左子树平衡了
            //从左子树的右子树获取右子树.
            TplNode *pNewRoot;
            //如果左子树的右子树存在
            TplNode *pLRNode = m_pLeftNode->m_pRightNode;
            if (pLRNode)
            {
                //并且其大小小于1/2,
                if (pLRNode->m_nodeSize <= (m_pLeftNode->m_nodeSize >>1))
                {
                    pNewRoot = m_pLeftNode;
                    //直接删除整个左子树的右子树
                    m_pLeftNode->m_pRightNode = this;
                    m_pLeftNode->m_pParent    = m_pParent;
                    m_pLeftNode->m_nodeSize   = m_nodeSize;
                    if (m_pRightNode)
                    {
                        m_nodeSize = m_pRightNode->GetTreeSize();
                        m_nodeSize += pLRNode->GetTreeSize();
                    }
                    else
                    {
                        m_nodeSize =pLRNode->GetTreeSize();
                    }
                    m_pParent = m_pLeftNode;
                    m_pLeftNode = pLRNode;
                    pLRNode->m_pParent = this; //完了！
                    return pNewRoot;
                }
                else
                {
                    //平衡左子树的右子树的子节点
                    int nBalance = pLRNode->NeedBalance();
                    if (nBalance>0)
                    {
                        m_pLeftNode->m_pRightNode = pLRNode->MigrateLeft2Right();
                        pLRNode = m_pLeftNode->m_pRightNode;
                    }
                    else if (nBalance < 0)
                    {
                        m_pLeftNode->m_pRightNode = pLRNode->MigrateRight2Left();
                        pLRNode = m_pLeftNode->m_pRightNode;
                    }
                    //平衡了，使用左子树的右子树的右子树
                    TplNode *pLRRNode = pLRNode->m_pRightNode;//->m_pRightNode; //肯定是存在的
                    if (pLRRNode)
                    {
                        pNewRoot = pLRNode;
                        m_pLeftNode->m_pRightNode = pLRNode->m_pLeftNode;
                        if (pLRNode->m_pLeftNode)
                        {
                            pLRNode->m_pLeftNode->m_pParent = m_pLeftNode;
                        }
                        m_pLeftNode->m_nodeSize -= (pLRRNode->GetTreeSize()+1);
                        m_pLeftNode->m_pParent = pLRNode;

                        pLRNode->m_pRightNode = this;
                        pLRNode->m_pParent    = m_pParent;
                        pLRNode->m_nodeSize   = m_nodeSize;
                        pLRNode->m_pLeftNode  = m_pLeftNode;

                        pLRRNode->m_pParent = this;

                        m_pLeftNode = pLRRNode;
                        if (m_pRightNode)
                        {
                            m_nodeSize = m_pRightNode->GetTreeSize();
                            m_nodeSize += pLRRNode->GetTreeSize();
                        }
                        else
                        {
                            m_nodeSize =pLRRNode->GetTreeSize();
                        }
                        m_pParent = pLRNode;
                        
                        
                        return pNewRoot;
                    }
                    else
                    {
                        pNewRoot = pLRNode;
                        m_pLeftNode->m_pRightNode = pLRNode->m_pLeftNode;
                        if (pLRNode->m_pLeftNode)
                        {
                            pLRNode->m_pLeftNode->m_pParent = m_pLeftNode;
                        }
                        --m_pLeftNode->m_nodeSize;// -= (pLRRNode->GetTreeSize()+1);
                        m_pLeftNode->m_pParent = pLRNode;

                        pLRNode->m_pRightNode = this;
                        pLRNode->m_pParent    = m_pParent;
                        pLRNode->m_nodeSize   = m_nodeSize;
                        pLRNode->m_pLeftNode  = m_pLeftNode;

                        //pLRRNode->m_pParent = this;

                        m_pLeftNode = NULL;
                        if (m_pRightNode)
                        {
                            m_nodeSize = m_pRightNode->GetTreeSize();
                        }
                        else
                        {
                            m_nodeSize = 0;
                        }
                        m_pParent = pLRNode;
                        return pNewRoot;
                    }
                    //以左子树的右子树的右子树,为迁出节点
                }
            }
            else //if ((rightNodeSize) > ((leftNodeSize+1)<<1)) "+1"或者是叶节点 引起的
            {
                //直接平衡,把左子树提上来当root
                pNewRoot = m_pLeftNode;
                m_pLeftNode->m_pRightNode = this;
                m_pLeftNode->m_pParent    = m_pParent;
                m_pLeftNode->m_nodeSize   = m_nodeSize;
                if (m_pRightNode)
                {
                    m_nodeSize = m_pRightNode->GetTreeSize();
                }
                else
                {
                    m_nodeSize = 0;
                }
                m_pParent = m_pLeftNode;
                m_pLeftNode = NULL;
                return pNewRoot;
            }
            //得到了.
#ifdef _DEBUG
        }
        return NULL;
#endif
    }


public:

    TplNode<CItem,CKey> *GetNextSmallFirst(TplNode<CItem,CKey> *pNow)
    {
        if (!pNow)
        {
            return NULL;
        }
        TplNode<CItem,CKey> *pResult = NULL;
        if (CItem::GetKey() > pNow->GetKey())
        {
            if (m_pLeftNode)
            {
                pResult = m_pLeftNode->GetNextSmallFirst(pNow);
                if (!pResult)
                {
                    pResult = this;
                }
                return pResult;
            }
            else
            {
                return NULL;
            }
        }
        if (CItem::GetKey() == pNow->GetKey())
        {
            if (m_pRightNode)
            {
                return m_pRightNode->GetSmallest();
            }
            return NULL;
        }
        //else 
        if (!m_pRightNode)
        {
            return NULL;
        }
        else
        {
            return m_pRightNode->GetNextSmallFirst(pNow);
        }
    }

    TplNode<CItem,CKey> *FindNode(CKey& key);
    TplNode<CItem,CKey> *SafeAddNode(TplNode *pNode,TInt32&result)
    {
            TplNode<CItem,CKey> *pResult = this;
            if (FindNode(pNode->GetKey()))
            {
                result = FAIL;
            }
            else
            {
                result = SUCCESS;
                pResult = AddNode(pNode);
            }
            return pResult;
    }
private:
    //if return NULL, mean this node had already existed! U can't add again!
    TplNode<CItem,CKey> *AddNode(TplNode *pNode);
public:
    //high efficiency also use this in release.
    TplNode<CItem,CKey> *SafeReleaseNode(TplNode<CItem,CKey> *pNode,TInt32& result)
        {
            TplNode<CItem,CKey> *pResult = FindNode(pNode->GetKey());
            if (pResult == pNode)
            {
                result = SUCCESS;
                pResult = ReleaseNode(pNode->GetKey());
            }
            else
            {
                result = FAIL;
            }
            return pResult;
        }
private:
    //if the key does not exist, U wouldn't know about it.
    TplNode<CItem,CKey> *ReleaseNode(CKey& key);
public:
    void        Init()
                {
                    m_nodeSize = 0;

                    m_pLeftNode = NULL;
                    m_pRightNode = NULL;
                    m_pParent    = NULL;
#ifdef _DEBUG
                    Active();
#endif
                }
    void        UnInit()
                {
#ifdef _DEBUG
                    Deactive();
#endif
                    m_nodeSize = 0;
                    m_pLeftNode = NULL;
                    m_pRightNode = NULL;
                    m_pParent    = NULL;
                }
#ifdef _DEBUG
    bool        IsActived()
                {
                    return m_isActive;
                }
    void        Active()
                {
                    m_isActive = 1;
                }
    void        Deactive()
                {
                    m_isActive = 0;
                }
#endif
    void        Print(TInt32 etch,TInt32& count);
    typedef TInt32 (CItem::*_PFMSG)(TInt32); 
	typedef TInt32 (*_PFV)(CItem *pThis,void *);

	void		RunOnTreeMiddleFirst(_PFV ptr,void *pArg)
				{
					ptr(this,pArg);
					if (m_pLeftNode)
					{
						m_pLeftNode->RunOnTreeMiddleFirst(ptr,pArg);
					}
					if (m_pRightNode)
					{
						m_pRightNode->RunOnTreeMiddleFirst(ptr,pArg);
					}
				}

    void        RunOnTree(_PFMSG ptr,TInt32 arg)
                {
                    if (m_pLeftNode)
                    {
                        m_pLeftNode->RunOnTree(ptr,arg);
                    }
					(this->*ptr)(arg);
                    if (m_pRightNode)
                    {
                        m_pRightNode->RunOnTree(ptr,arg);
                    }
                }
    

    TInt32         GetTreeSize()
                {
                    return (m_nodeSize + 1);
                }
private:
                //如果为0，则子树平衡，>0,则需要向右迁移平衡，小于零需要向左迁移平衡.
    int         NeedBalance()
                {
                    TInt32 leftNodeSize;
                    TInt32 rightNodeSize;
                    if (m_pLeftNode)
                    {
                        leftNodeSize = m_pLeftNode->GetTreeSize();
                    }
                    else
                    {
                        leftNodeSize = 0;
                    }
                    if (m_pRightNode)
                    {
                        rightNodeSize = m_pRightNode->GetTreeSize();
                    }
                    else
                    {
                        rightNodeSize = 0;
                    }
                    if ((rightNodeSize) > ((leftNodeSize<<1)+2))
                    {
                        return (-1);
                    }
                    if ((leftNodeSize) > ((rightNodeSize<<1)+2)) 
                    {
                        return 1;
                    }
                    return 0;
                }

    bool        NeedRearrange(TBOOL bRightNode)
                {
                    //return false;
                    TInt32 leftNodeSize;
                    TInt32 rightNodeSize;
                    if (m_pLeftNode)
                    {
                        leftNodeSize = m_pLeftNode->GetTreeSize();
                    }
                    else
                    {
                        leftNodeSize = 0;
                    }
                    if (m_pRightNode)
                    {
                        rightNodeSize = m_pRightNode->GetTreeSize();
                    }
                    else
                    {
                        rightNodeSize = 0;
                    }
                    if (bRightNode)
                    {
                        /* balance = leftNodeSize - rightNodeSize;*/
                        if ((rightNodeSize) > ((leftNodeSize<<1)+2))
                        {
                            return true;
                        }
                        return false;
                    }
                    else // the right node case
                    {
                        /* balance = rightNodeSize - leftNodeSize;*/

                        if ((leftNodeSize) > ((rightNodeSize<<1)+2)) 
                        {
                            return true;
                        }
                        return false;
                    }
//                     TInt32 balance = 0;
//                     TInt32 totalNum = (GetTreeSize()>>2);
//                     //totalNum = totalNum>>2;
//                     totalNum += 4;
//                     if (left_node == nextAddNode)
//                     {
//                         balance = leftNodeSize - rightNodeSize;
//                     }
//                     else // the right node case
//                     {
//                         balance = rightNodeSize - leftNodeSize;
//                     }
//                     /*
//                     if (balance > 64)
//                     {
//                         return true;
//                     }
//                     */
//                     if (balance > totalNum)
//                     {
//                         return true;
//                     }
//                     return false;
                }

    void        AddLeftNodeNum()
                {
                    m_nodeSize ++;
                }

    void        DelLeftNodeNum()
                {
                    m_nodeSize --;
                }

    void        AddRightNodeNum()
                {
                    m_nodeSize ++;
                }

    void        DelRightNodeNum()
                {
                    m_nodeSize --;
                }
public:
    TInt32         GetMaxHeight()
                {
                    TInt32 lHt = 0;
                    TInt32 rHt = 0;
                    if (m_pLeftNode)
                    {
                        lHt = m_pLeftNode->GetMaxHeight();
                    }
                    if (m_pRightNode)
                    {
                        rHt = m_pRightNode->GetMaxHeight();
                    }
                    if (lHt > rHt)
                    {
                        return (++lHt);
                    }
                    else
                    {
                        return (++rHt);
                    }
                }
private:
    bool IsLeftChild()
    {
        if (m_pParent)
        {
            if (m_pParent->m_pLeftNode == this)
            {
                return true;
            }
        }
        return false;
    }
    bool IsRoot()
    {
        if (m_pParent)
        {
            return false;
        }
        return true;
    }
    bool IsRightChild()
    {
        if (m_pParent)
        {
            if (m_pParent->m_pRightNode == this)
            {
                return true;
            }
        }
        return false;
    }
    TInt32        CheckNode()
    {
        TInt32 totalNode = 0;
        if (m_pLeftNode)
        {
            if(m_pLeftNode->FindNode(CItem::GetKey()))
            {
                printf("check tree failed, find self in the m_pLeftNode \n");
                TInt32 count = 0;
                Print(1,count);
                return FAIL;
            }
            if (!(CItem::GetKey() > m_pLeftNode->GetKey()))
            {
                printf("check tree failed, left node key is not smaller than root!\n");
                TInt32 count = 0;
                Print(1,count);
                return FAIL;
            }
            totalNode += (m_pLeftNode->GetTreeSize());
            if (m_pLeftNode->m_pParent != this)
            {
                printf("check tree failed, (m_pLeftNode->m_pParent in correct!!\n");
                
                return FAIL;
            }
        }
        if (m_pRightNode)
        {
            if (m_pRightNode->FindNode(CItem::GetKey()))
            {
                printf("check tree failed, find self in the m_pRightNode \n");
                TInt32 count = 0;
                Print(1,count);
                return FAIL;
            }
            if (!(CItem::GetKey() < m_pRightNode->GetKey()))
            {
                printf("check tree failed, right node key is not bigger than root!\n");
                TInt32 count = 0;
                Print(1,count);
                return FAIL;
            }
            totalNode += (m_pRightNode->GetTreeSize());
            if (m_pRightNode->m_pParent != this)
            {
                printf("check tree failed, m_pRightNode->m_pParent is incorrect!\n");
                return FAIL;
            }
        }
        if (totalNode != (GetTreeSize()-1))
        {
            printf("check tree failed, tree size incorrect!\n");
            TInt32 count = 0;
            Print(1,count);
            return FAIL;
        }
        if (m_pParent)
        {
            if (m_pParent->GetKey() == GetKey())
            {
                printf("Check tree failed ,parent's key is the same to this!\n");
                return FAIL;
            }
//             if (m_pParent->GetKey() > GetKey())
//             {
//                 if (m_pParent->m_pLeftNode != this)
//                 {
//                     printf("check tree failed ,the tree parent is incorrect!\n");
//                     return FAIL;
//                 }
//             }
//             else
//             {
//                 if (m_pParent->m_pRightNode != this)
//                 {
//                     printf("check tree failed, the tree parent is incorrect!\n");
//                     return FAIL;
//                 }
//             }
        }
        return SUCCESS;
    }
public:
    TplNode<CItem,CKey>* GetBigest()
                            {
                            #ifdef _DEBUG
                                if (this == m_pRightNode)
                                {
                                    return NULL;
                                }
                            #endif
                                if (m_pRightNode)
                                {
                                    return m_pRightNode->GetBigest();
                                }
                                return this;
                            }

    TplNode<CItem,CKey>* GetSmallest()
                            {
                            #ifdef _DEBUG
                                if (this == m_pLeftNode)
                                {
                                    return NULL;
                                }
                            #endif
                                if (m_pLeftNode)
                                {
                                    return m_pLeftNode->GetSmallest();
                                }
                                return this;
                            }

    TplNode<CItem,CKey>* GetRoot()
                            {
                                if (m_pParent)
                                {
                                    return m_pParent->GetRoot();
                                }
                                return this;
                            }

public:
    class Iterator
    {
    private:
        TplNode<CItem,CKey>* m_pNow;
		//TplNode<CItem,CKey>* m_pRoot;
    public:
        Iterator()
        {
            m_pNow = NULL;
            //m_pRoot = NULL;
        }
        Iterator(TplNode<CItem,CKey>* pNow)
        {
            m_pNow = pNow;
        }
        TInt32 Init(TplNode<CItem,CKey>* pNow)
        {
            if(!pNow)
            {
                return NULL_POINTER;
            }
            //m_pRoot = pNow;
			m_pNow = pNow;
            return SUCCESS;
        }
        //前缀++，没有后缀++
        Iterator &operator ++()
        {
            //m_pNow = m_pRoot->GetNextSmallFirst(m_pNow);
            TplNode<CItem,CKey>* pTmp = m_pNow;
            TplNode<CItem,CKey>* pLast = NULL;
            //m_pNow = NULL;
            while (pTmp)
            {
                if ((pTmp->m_pRightNode) && (pTmp->m_pRightNode != pLast))
                {
                    m_pNow = pTmp->m_pRightNode->GetSmallest();
                    return *this;
                }
                else
                {
                    if (pTmp->IsLeftChild())
                    {
                        m_pNow = pTmp->m_pParent;
                        return *this;
                    }
                    else
                    {
                        pLast = pTmp;
                        pTmp = pTmp->m_pParent;
                    }
                }
            }
            m_pNow = NULL;
            return *this;
        }
        Iterator &operator --()
        {
            TplNode<CItem,CKey>* pTmp = m_pNow;
            TplNode<CItem,CKey>* pLast = NULL;
            m_pNow = NULL;
            while (pTmp)
            {
                if ((pTmp->m_pLeftNode) && (pTmp->m_pLeftNode != pLast))
                {
                    m_pNow = pTmp->m_pLeftNode->GetBigest();
                    return *this;
                }
                else
                {
                    if (pTmp->IsRightChild())
                    {
                        m_pNow = pTmp->m_pParent;
                        return *this;
                    }
                    else
                    {
                        pLast = pTmp;
                        pTmp = pTmp->m_pParent;
                    }
                }
            }
            m_pNow = NULL;
            return *this;
        }
        CItem*      operator ->()
        {
            return m_pNow;
        }
        bool        operator == (Iterator& itor)
        {
            if (itor.m_pNow == m_pNow)
            {
                return true;
            }
            return false;
        }
		TplNode<CItem,CKey>* GetItem()
		{
			return m_pNow;
		}
		TInt32 Size()
		{
		    if (m_pNow)
		    {
		        return (m_pNow->GetRoot()->GetTreeSize());
		    }
		    return 0;
		}
		bool IsNull()
		{
		    return (m_pNow == NULL); 
		}
    };

};


template<class CItem,class CKey>
void TplNode<CItem,CKey>::Print(TInt32 etch,TInt32& count)
{
    TInt32 needPlusSign = 0;
    if (m_pLeftNode)
    {
        m_pLeftNode->Print(etch+1,count);
    }
    else
    {
        if (m_pRightNode)
        {
            needPlusSign = 1;
           /* for(TInt32 i=0;i<etch+1;i++)
            {
                cout<<"    ";
            }
            cout<<"-["<<etch + 1<<"]"<<"NULL"<<endl;*/
        }
    }

    for(TInt32 i=0;i<etch;i++)
    {
        cout<<"    ";
    }

    cout<<"-["<<etch<<"]";
    if (needPlusSign)
    {
        cout<<"+  ";
    }
    cout<<CItem::GetKey()<<"("<<count++<<") nodeSize :"<<m_nodeSize<<endl;

    if (m_pRightNode)
    {
        m_pRightNode->Print(etch+1,count);
    }
    else
    {
        if (m_pLeftNode)
        {
            /*for(TInt32 i=0;i<etch+1;i++)
            {
                cout<<"    ";
            }
            cout<<"-["<<etch + 1<<"]"<<"NULL"<<endl;*/

        }
    }
}



template<class CItem,class CKey>
TplNode<CItem,CKey> *TplNode<CItem,CKey>::FindNode(CKey& key)
{
    if (CItem::GetKey() == key)
    {
        return this;
    }
    if (CItem::GetKey() > key)
    {
        if (m_pLeftNode)
        {
            return m_pLeftNode->FindNode(key);
        }
        return NULL;
    }
    if (m_pRightNode)
    {
        return m_pRightNode->FindNode(key);
    }
    return NULL;
}

template<class CItem,class CKey>
TplNode<CItem,CKey> *TplNode<CItem,CKey>::AddNode(TplNode *pNode)
{
//#ifdef _DEBUG
    if (this == pNode)
    {
        return this;
    }
//#endif
#ifdef _DEBUG
    if (NULL == pNode)
    {
        
        CheckNode();

        return NULL;
    }
    pNode->Init();
    if (CItem::GetKey() == pNode->GetKey())
    {
        //almost never happened!
        #ifdef _NEED_TREE_CHECK
        CheckNode();
        #endif
        return this;
    }
#endif
    if (CItem::GetKey() > pNode->GetKey())
    {
        if (NeedRearrange(FALSE))
        {
#ifdef _DEBUG
            if (NULL == m_pLeftNode)
            {
                //sth wrong!! this should never happened!
                m_pLeftNode = pNode;
                pNode->m_pParent = this;
                AddLeftNodeNum();
                #ifdef _NEED_TREE_CHECK
                CheckNode();
                #endif
                return this;
            }
#endif
            //重新修整树
               TplNode<CItem,CKey>* pNewRoot = MigrateLeft2Right();
//             TplNode<CItem,CKey>* pNewRoot = m_pLeftNode;
//             m_pLeftNode    = m_pLeftNode->m_pRightNode;
//             pNewRoot->m_pRightNode = this;
//             m_nodeSize = 0;
//             
//             //修整m_pParent
//             pNewRoot->m_pParent = m_pParent;
//             this->m_pParent = pNewRoot;
//             if (m_pLeftNode)
//             {
//                 m_pLeftNode->m_pParent = this;
//             }
//             //修整结束
//             
//             if(m_pLeftNode)
//             {
//                 m_nodeSize += m_pLeftNode->GetTreeSize();
//             }
//             if (m_pRightNode)
//             {
//                 m_nodeSize += m_pRightNode->GetTreeSize();
//             }
//             pNewRoot->m_nodeSize = 0;
//             if (pNewRoot->m_pLeftNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pLeftNode->GetTreeSize();
//             }
//             if (pNewRoot->m_pRightNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pRightNode->GetTreeSize();
//             }
//             //结束
//             
#ifdef _NEED_TREE_CHECK
               pNewRoot->CheckTree();
#endif
//             //if (pNewRoot)
            {
                if (pNewRoot->GetKey() > pNode->GetKey())
                {
                    if (pNewRoot->m_pLeftNode)
                    {
                        pNewRoot->m_pLeftNode = pNewRoot->m_pLeftNode->AddNode(pNode);
                    }
                    else
                    {
                        pNode->m_pParent = pNewRoot;
                        pNewRoot->m_pLeftNode = pNode;
                    }
                    pNewRoot->AddLeftNodeNum();
                }
                else
                {
                    pNewRoot->m_pRightNode = pNewRoot->m_pRightNode->AddNode(pNode);
                    pNewRoot->AddRightNodeNum();
                }
            }
            #ifdef _NEED_TREE_CHECK
            pNewRoot->CheckTree();
            #endif
            return pNewRoot;
            
            /*
            TplNode<CItem,CKey>* pNewRoot = m_pLeftNode->GetBigest();

            if ((pNode->GetKey()) > (pNewRoot->GetKey()))
            {
                //the new added node would be the root!
                pNewRoot = pNode;
            }
            else
            {
                m_pLeftNode = m_pLeftNode->ReleaseNode(pNewRoot->GetKey());
                #ifdef _DEBUG
                if (m_pLeftNode)
                {
                    m_pLeftNode->CheckTree();
                }
                #endif
                //BEGIN ADD 01-04-2009 S0032 TDS00034
                if (m_pLeftNode)
                {
                //END ADD 01-04-2009 S0032 TDS00034
                    m_pLeftNode = m_pLeftNode->AddNode(pNode);
                    #ifdef _DEBUG
                    m_pLeftNode->CheckTree();
                    #endif
                //BEGIN ADD 01-04-2009 S0032 TDS00034
                }
                else
                {
                    m_pLeftNode = pNode;

                }
                //END ADD 01-04-2009 S0032 TDS00034

            }
            pNewRoot->m_pLeftNode = m_pLeftNode;
            pNewRoot->m_pRightNode = m_pRightNode;
            pNewRoot->m_nodeSize = m_nodeSize;


            if (pNewRoot->m_pRightNode)
            {

                pNewRoot->m_pRightNode = pNewRoot->m_pRightNode->AddNode(this);

            }
            else
            {
                

                //BEGIN ADD 01-04-2009 S0032 TDS00036
                this->Init();
                //END ADD 01-04-2009 S0032 TDS00036
                pNewRoot->m_pRightNode = this;
            }

            pNewRoot->AddRightNodeNum();
            */
#ifdef _NEED_TREE_CHECK
            pNewRoot->CheckTree();
#endif
            return pNewRoot;

        }
        else
        {
            if (m_pLeftNode)
            {
                m_pLeftNode = m_pLeftNode->AddNode(pNode);
#ifdef _NEED_TREE_CHECK
                m_pLeftNode->CheckTree();
#endif
            }
            //BEGIN ADD 01-04-2009 S0032 TDS00035
            else
            {
            //END ADD 01-04-2009 S0032 TDS00034
                pNode->m_pParent = this;
                m_pLeftNode = pNode;
            }
            AddLeftNodeNum();
#ifdef _NEED_TREE_CHECK
            CheckTree();
#endif
            return this;
        }
    }

    //add to right node.
    if (NeedRearrange(TRUE))
    {
#ifdef _DEBUG
        if (NULL == m_pRightNode)
        {

            m_pRightNode = pNode;
            AddRightNodeNum();
#ifdef _NEED_TREE_CHECK
            CheckNode();
#endif
            return this;
        }
#endif
        //重新修正树
        TplNode<CItem,CKey>* pNewRoot = MigrateRight2Left();
//         TplNode<CItem,CKey>* pNewRoot = m_pRightNode;
//         m_pRightNode    = m_pRightNode->m_pLeftNode;
//         pNewRoot->m_pLeftNode = this;
//         m_nodeSize = 0;
//         
//         //修整m_pParent
//         pNewRoot->m_pParent = m_pParent;
//         this->m_pParent = pNewRoot;
//         if (m_pRightNode)
//         {
//             m_pRightNode->m_pParent = this;
//         }
//         //修整结束
//         
//         if(m_pLeftNode)
//         {
//             m_nodeSize += m_pLeftNode->GetTreeSize();
//         }
//         if (m_pRightNode)
//         {
//             m_nodeSize += m_pRightNode->GetTreeSize();
//         }
//         pNewRoot->m_nodeSize = 0;
//         if (pNewRoot->m_pLeftNode)
//         {
//             pNewRoot->m_nodeSize += pNewRoot->m_pLeftNode->GetTreeSize();
//         }
//         if (pNewRoot->m_pRightNode)
//         {
//             pNewRoot->m_nodeSize += pNewRoot->m_pRightNode->GetTreeSize();
//         }
#ifdef _NEED_TREE_CHECK
        pNewRoot->CheckTree();
#endif
        //if (pNewRoot)
        {
            if (pNewRoot->GetKey() > pNode->GetKey())
            {
                pNewRoot->m_pLeftNode = pNewRoot->m_pLeftNode->AddNode(pNode);
                pNewRoot->AddLeftNodeNum();
            }
            else
            {
                if (pNewRoot->m_pRightNode)
                {
                    pNewRoot->m_pRightNode = pNewRoot->m_pRightNode->AddNode(pNode);
                }
                else
                {
                    pNode->m_pParent = pNewRoot;
                    pNewRoot->m_pRightNode = pNode;
                }
                pNewRoot->AddRightNodeNum();
            }
        }
        #ifdef _NEED_TREE_CHECK
        pNewRoot->CheckTree();
        #endif
        return pNewRoot;
        /*
        TplNode<CItem,CKey>* pNewRoot = m_pRightNode->GetSmallest();
        if ((pNode->GetKey()) < (pNewRoot->GetKey()))
        {
            pNewRoot = pNode;
        }
        else
        {
            //BEGIN ADD 01-04-2009 s0032 TDS00037
            m_pRightNode = m_pRightNode->ReleaseNode(pNewRoot->GetKey());

#ifdef _DEBUG
            if (m_pRightNode)
            {
                m_pRightNode->CheckTree();
            }
#endif
            if (m_pRightNode)
            {
            //END ADD 01-04-2009 s0032 TDS00037
                m_pRightNode = m_pRightNode->AddNode(pNode);

#ifdef _DEBUG
                m_pRightNode->CheckTree();
#endif
            //BEGIN ADD 01-04-2009 s0032 TDS00037
            }
            else
            {
                m_pRightNode = pNode;
            }
            //END ADD 01-04-2009 s0032 TDS00037
        }

        //BEGIN ADD 01-04-2009 s0032 TDS00038
        //pNode->m_info = m_info;
        pNewRoot->m_pLeftNode = m_pLeftNode;
        pNewRoot->m_pRightNode = m_pRightNode;
        pNewRoot->m_nodeSize = m_nodeSize;
        //END ADD 01-04-2009 s0032 TDS00038
        if (pNewRoot->m_pLeftNode)
        {
            pNewRoot->m_pLeftNode = pNewRoot->m_pLeftNode->AddNode(this);
#ifdef _DEBUG
            pNewRoot->m_pLeftNode->CheckTree();
#endif
        }
        else
        {
            pNewRoot->m_pLeftNode = this;
            //BEGIN ADD 01-04-2009 S0032 TDS00036
            Init();
            //END ADD 01-04-2009 S0032 TDS00036
        }
        pNewRoot->AddLeftNodeNum();
#ifdef _DEBUG
        pNewRoot->CheckTree();
#endif
        return pNewRoot;
        */
    }
    else
    {
        if (m_pRightNode)
        {
            m_pRightNode = m_pRightNode->AddNode(pNode);
        }
        //BEGIN ADD 01-04-2009 S0032 TDS00035
        else
        {
        //END ADD 01-04-2009 S0032 TDS00035
            pNode->m_pParent = this;
            m_pRightNode = pNode;
        }
        AddRightNodeNum();
#ifdef _NEED_TREE_CHECK
        CheckNode();
#endif
        return this;
    }
}

template<class CItem,class CKey>
TplNode<CItem,CKey> *TplNode<CItem,CKey>::ReleaseNode(CKey& key)
{
    //make sure the key does existed!
    //TplNode *pResult = FindNode(key);
    if (CItem::GetKey() == key)
    {
        // release this node;
        //if this node is the leaf, than just release this node;
        if ((NULL == m_pLeftNode) && (NULL == m_pRightNode))
        {
            UnInit();
            return NULL;
        }
        if (NULL == m_pLeftNode)
        {
            TplNode<CItem,CKey> *pResult = m_pRightNode;
            m_pRightNode->m_pParent = m_pParent;
            //this cann't be null;
            UnInit();
            #ifdef _NEED_TREE_CHECK
            pResult->CheckTree();
            #endif
            return pResult;
        }
        if (NULL == m_pRightNode)
        {
            TplNode<CItem,CKey> *pResult = m_pLeftNode;
            m_pLeftNode->m_pParent = m_pParent;
            UnInit();
            #ifdef _NEED_TREE_CHECK
            pResult->CheckTree();
            #endif
            return pResult;
        }
        //other cases!
        TplNode<CItem,CKey>* pNewRoot = NULL;

        TInt32 balanceNum = m_pLeftNode->GetTreeSize();
        balanceNum -= m_pRightNode->GetTreeSize();

        if (balanceNum < 0)
        {
            pNewRoot = m_pRightNode->GetSmallest();
            m_pRightNode = m_pRightNode->ReleaseNode(pNewRoot->GetKey());
#ifdef _NEED_TREE_CHECK
            if (m_pRightNode)
            {
                m_pRightNode->CheckTree();
            }
#endif
            DelRightNodeNum();
        }
        else
        {
            pNewRoot = m_pLeftNode->GetBigest();
            m_pLeftNode = m_pLeftNode->ReleaseNode(pNewRoot->GetKey());
#ifdef _NEED_TREE_CHECK
            if (m_pLeftNode)
            {
                m_pLeftNode->CheckTree();
            }
#endif
            DelLeftNodeNum();
        }
        pNewRoot->m_nodeSize = m_nodeSize;
        pNewRoot->m_pParent = m_pParent;
        pNewRoot->m_pLeftNode  = m_pLeftNode;
        pNewRoot->m_pRightNode = m_pRightNode;
        if (m_pLeftNode)
        {
            m_pLeftNode->m_pParent = pNewRoot;
        }
        if (m_pRightNode)
        {
            m_pRightNode->m_pParent = pNewRoot;
        }
        UnInit();
        #ifdef _NEED_TREE_CHECK
        pNewRoot->CheckTree();
        #endif
        return pNewRoot;

        //return the new root of this subtree.
    }
    //otherwist just release the node in the tree and return this itself, because the root is not changed.
    if (CItem::GetKey() > key)
    {
        //equals to minus left node num.
#ifdef _DEBUG
        // m_pLeftNode must be existed!
        if (NULL == m_pLeftNode)
        {
            //sth wrong!!!
            //cout<<"(TplMap::ReleaseItem)can not find the node!"<<endl;
            #ifdef _NEED_TREE_CHECK
            CheckTree();
        #endif
            return this;
        }
#endif
        TplNode<CItem,CKey>*   pNewRoot = this;
        //minus a left node equals to add a right node
        if (NeedRearrange(TRUE))
        {
            /*
            #ifdef _DEBUG
            if (m_pRightNode)
            {
                #endif
                pNewRoot = m_pRightNode->GetSmallest();
                m_pRightNode = m_pRightNode->ReleaseNode(pNewRoot->GetKey());
                #ifdef _DEBUG
                if (m_pRightNode)
                {
                    m_pRightNode->CheckTree();
                }
                #endif
                DelRightNodeNum();
                pNewRoot->m_pRightNode = m_pRightNode;
                pNewRoot->m_pLeftNode  = m_pLeftNode;
                pNewRoot->m_nodeSize   = m_nodeSize;
                #ifdef _DEBUG
                if (!pNewRoot->m_pLeftNode)
                {
                    TInt32 *p = NULL;
                    *p = 1;
                    cout<<"pNewRoot->m_pLeftNode should not be null!"<<endl;
                }
                else
                {
                #endif
                pNewRoot->m_pLeftNode  = pNewRoot->m_pLeftNode->AddNode(this);
#ifdef _DEBUG
                pNewRoot->m_pLeftNode->CheckTree();
#endif
                pNewRoot->AddLeftNodeNum();
                #ifdef _DEBUG
                }
                #endif


                #ifdef _DEBUG
            }
            else
            {
                TInt32 *p = NULL;
                *p = 1;
                cout<<"release node , something is wrong, the right node should not be null!"<<endl;
            }
            #endif
            */
#ifdef _DEBUG
            if (NULL == m_pRightNode)
            {
                return this;
            }
#endif
            //重整树
            TplNode<CItem,CKey>* pNewRoot = MigrateRight2Left();
//             TplNode<CItem,CKey>* pNewRoot = m_pRightNode;
//             m_pRightNode    = m_pRightNode->m_pLeftNode;
//             pNewRoot->m_pLeftNode = this;
//             m_nodeSize = 0;
//             
//             //重整m_pParent
//             pNewRoot->m_pParent = m_pParent;
//             this->m_pParent = pNewRoot;
//             if (m_pRightNode)
//             {
//                 m_pRightNode->m_pParent = this;
//             }
//             
//             if(m_pLeftNode)
//             {
//                 m_nodeSize += m_pLeftNode->GetTreeSize();
//             }
//             if (m_pRightNode)
//             {
//                 m_nodeSize += m_pRightNode->GetTreeSize();
//             }
//             pNewRoot->m_nodeSize = 0;
//             if (pNewRoot->m_pLeftNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pLeftNode->GetTreeSize();
//             }
//             if (pNewRoot->m_pRightNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pRightNode->GetTreeSize();
//             }
#ifdef _NEED_TREE_CHECK
            pNewRoot->CheckTree();
#endif
            pNewRoot = pNewRoot->ReleaseNode(key);
            #ifdef _NEED_TREE_CHECK
            pNewRoot->CheckTree();
            #endif
            return pNewRoot;

        }
        pNewRoot->m_pLeftNode = pNewRoot->m_pLeftNode->ReleaseNode(key);
        
        pNewRoot->DelLeftNodeNum();
        #ifdef _NEED_TREE_CHECK
        pNewRoot->CheckTree();
        #endif
        return pNewRoot;
    }
    else
    {
        if (NULL == m_pRightNode)
        {
            //sth wrong!!!
            #ifdef _NEED_TREE_CHECK
            CheckTree();
            #endif
            return this;
        }
        TplNode<CItem,CKey>*   pNewRoot = this;
        //minus a right node equals to add a left node
        if (NeedRearrange(FALSE))
        {
#ifdef _DEBUG
            if (NULL == m_pLeftNode)
            {
                //sth wrong!! this should never happened!
                //m_pLeftNode = pNode;
                //DelLeftNodeNum();
                #ifdef _NEED_TREE_CHECK
                CheckNode();
                #endif
                return this;
            }
#endif
            //重整树
            TplNode<CItem,CKey>* pNewRoot = MigrateLeft2Right();
//             TplNode<CItem,CKey>* pNewRoot = m_pLeftNode;
//             m_pLeftNode    = m_pLeftNode->m_pRightNode;
//             pNewRoot->m_pRightNode = this;
//             m_nodeSize = 0;
//             //重整m_pParent
//             pNewRoot->m_pParent = m_pParent;
//             this->m_pParent = pNewRoot;
//             if (m_pLeftNode)
//             {
//                 m_pLeftNode->m_pParent = this;
//             }
//             
//             if(m_pLeftNode)
//             {
//                 m_nodeSize += m_pLeftNode->GetTreeSize();
//             }
//             if (m_pRightNode)
//             {
//                 m_nodeSize += m_pRightNode->GetTreeSize();
//             }
//             pNewRoot->m_nodeSize = 0;
//             if (pNewRoot->m_pLeftNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pLeftNode->GetTreeSize();
//             }
//             if (pNewRoot->m_pRightNode)
//             {
//                 pNewRoot->m_nodeSize += pNewRoot->m_pRightNode->GetTreeSize();
//             }
            pNewRoot = pNewRoot->ReleaseNode(key);
            #ifdef _NEED_TREE_CHECK
            pNewRoot->CheckTree();
            #endif
            return pNewRoot;
        }
        pNewRoot->m_pRightNode = pNewRoot->m_pRightNode->ReleaseNode(key);
        pNewRoot->DelRightNodeNum();
        #ifdef _NEED_TREE_CHECK
        pNewRoot->CheckTree();
        #endif
        return pNewRoot;
    }
    #ifdef _NEED_TREE_CHECK
        CheckTree();
    #endif
    //cout<<"TplMap::ReleaseItem:774 this should never happened!"<<endl;
    return this;
}


//make sure class CKey reload ">" ,"<" and "==";
template<class CItem, class CKey>
class TplMap
{
private:

    //TInt32                   m_nMaxSize;
    //TplNode<CItem,CKey>*     m_pItem;
    CPool<TplNode<CItem,CKey> > *m_pPool;
    TplNode<CItem,CKey>*     m_pTree;
private:
    
public:
	TplNode<CItem,CKey>*     GetRoot()
	{
		return m_pTree;
	}
public:
    void CheckTree()
    {
        if(m_pTree)
        {
            m_pTree->CheckTree();
        }
    }
	TInt32 GetActivedSize()
	{
		if (m_pTree)
		{
			return (m_pTree->m_nodeSize + 1);
		}
		return 0;
	}
    
private:
public:
    typedef TInt32 (CItem::*_PFMSG)(TInt32);
    //Begin是有效的
    TplNode<CItem,CKey> *First()
    {
        if (m_pTree)
        {
             return m_pTree->GetSmallest();
        }
        return NULL;
    }
    //end也是有效的
    TplNode<CItem,CKey> *Last()
    {
        if (m_pTree)
        {
            return m_pTree->GetBigest();
        }
        return NULL;
    }
    
    void RunOnTree(_PFMSG ptr,TInt32 arg)
    {
		if (m_pTree)
		{
			m_pTree->RunOnTree(ptr, arg);
		}
    }
    TplMap()
    {
        m_pPool = NULL;
        m_pTree = NULL;
    };
    ~TplMap()
    {
        UnInit();
    }
    TInt32 Init(CPool<TplNode<CItem,CKey> > *pPool)
    {
        m_pPool = pPool;
        return SUCCESS;
    }
    void UnInit()
    {
    }
    CItem *PrepareItem()
    {
        TplNode<CItem,CKey>*pItem = m_pPool->GetMem();
        pItem->Init();
        return pItem;
    }
    //first get the Item
    //CItem        *GetItem(TInt32& result,CKey* pKey=NULL);

    /*
    Iterator     Begin()
                 {
                     TplMap<CItem,CKey>Iterator itor;
                     itor.m_pMap   = this;
                     itor.m_pNode  = m_pItem;
                     return itor;
                 }

    Iterator     End()
                 {
                     TplMap<CItem,CKey>Iterator itor;
                     itor.m_pMap   = this;
                     itor.m_pNode  = m_pItem + m_size;
                     return itor;
                 }
    */

/*    CItem        *FindItem(TInt32 idx);*/
    TInt32          ReleaseItem(CItem * pItem)
    {
        TplNode<CItem,CKey>* pNode = (TplNode<CItem,CKey>*)pItem;
        if (m_pPool->ReleaseMem(pNode))
        {
            return SUCCESS;
        }
        return FAIL;
    }
    
    //必须是从map来的
    TInt32      RemoveFromTree(CItem *pItem);
    TInt32      AddInTree(CItem* pItem);

    TplNode<CItem,CKey>  *GetItemByKey(CKey rKey)
    {
        if  (NULL == m_pTree)
        {
            return NULL;
        }
        return m_pTree->FindNode(rKey);
    }
    TInt32         GetFreeSize()
                {
                    return m_tPool.GetFreeNr();
                }

    TInt32         PrintTree()
                {
                    TInt32 count = 0;
                    if (m_pTree)
                    {
                        m_pTree->Print(0,count);
                    }
                    else
                    {
                        cout<<"The tree is empty!"<<endl;
                    }
                    return SUCCESS;
                }
};

// template<class CItem, class CKey>
// TInt32 TplMap<CItem,CKey>::Init(TInt32 size)
// {
//     if (size < 0)
//     {
//         return INPUT_PARA_ERROR;
//     }
//     // 0 ~ 1G memory was used as system area.
//     //may have warnings, but it's ok.
//     m_pTree = NULL;
//     return m_tPool.InitPool(size);
// }
// 
// template<class CItem, class CKey>
// CItem *TplMap<CItem,CKey>::GetItem(TInt32& result,CKey* pKey)
// {
// //     if (pKey)
// //     {
// //         if (GetItemByKey(pKey))
// //         {
// //             result = KEY_ALREADY_EXIST;
// //             return NULL;
// //         }
// //     }
//     //get a new node;
//     TplNode<CItem,CKey>* pResult = m_tPool.GetMem();
//     if (pResult)
//     {
//         pResult->Init();
//     }
//     else
//     {
//         result = OUT_OF_MEM;
//         return NULL;
//     }
//     if (pKey)
//     {
//         pResult->GetKey() = *pKey;
//         if (m_pTree)
//         {
//             
//         }
//         else
//         {
//             m_pTree = pResult;
//         }
//     }
//     result = SUCCESS;
//     return pResult;
// }
// 




// template<class CItem, class CKey>
// CItem *TplMap<CItem,CKey>::FindItem(TInt32 idx)
// {
//     if ((idx > (m_size - 1))||(idx < 0))
//     {
//         return NULL;
//     }
//     return m_pItem + idx;
// }



template<class CItem, class CKey>
TInt32 TplMap<CItem,CKey>::RemoveFromTree(CItem * pItem)
{
    if (NULL == pItem)
    {
        return NULL_POINTER;
    }
    //we do not use dynamic_cast to check if pItem belongs to this list, because the CItem has virtual func, otherwise it wouldn't has v_table.
    TplNode<CItem,CKey> *pNew = static_cast< TplNode<CItem,CKey>* >(pItem);

    //TInt32 offset = ((char*)pNew - (char*)m_pItem);


//     if ((pNew >= (m_pItem + m_size))||(offset < 0)||(0 != (offset % sizeof( TplNode<CItem,CKey>* ))))
//     {
//         return NOT_BELONG_TO_THIS_CAPSULA;
//     }
    //delete the pItem in the tree;
    TInt32 nResult = SUCCESS;
    if (m_pTree)
    {
        m_pTree = m_pTree->SafeReleaseNode(pNew,nResult);
    }
    if (nResult < SUCCESS)
    {
        return nResult;
    }
    //force upper casting, because we know it's ok.
    return SUCCESS;
}

template<class CItem, class CKey>
TInt32 TplMap<CItem,CKey>::AddInTree(CItem* pItem)
{
    if ((NULL == pItem))
    {
        return NULL_POINTER;
    }

    TplNode<CItem,CKey> *pNew = static_cast< TplNode<CItem,CKey>* >(pItem);
//     TInt32 offset = ((char*)pNew - (char*)m_pItem);
// 
// 
//     if ((offset < 0)||(0 != (offset % sizeof( TplNode<CItem,CKey>* ))))
//     {
//         return NOT_BELONG_TO_THIS_CAPSULA;
//     }
    TInt32 nResutl;
    if (m_pTree)
    {
        m_pTree = m_pTree->SafeAddNode(pNew,nResutl);
    }
    else
    {
        m_pTree = pNew;
        nResutl = SUCCESS;
    }
    return nResutl;
}



}
#endif

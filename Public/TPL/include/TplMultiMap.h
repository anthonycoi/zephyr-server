/*************************************************
  Copyright (C), 1988-1999, Zephyr Platform
  File name:      TplMultiMap.h
  Author:         Zephyr Shannon
  Version:        V1.2
  Date:           12-22-2009
  Description:    Map template,duplicated key is allowed!
  Others:         none
  Function List:  
      It looks like std::map, but more efficient,
      I take consider of the worst condition.
      And for the most important thing: U can read and debug it!
  History:
*************************************************/
#ifndef _ZEPHYR_PUBLIC_TPL_MULTI_MAP_TEMPLATE_H_
#define _ZEPHYR_PUBLIC_TPL_MULTI_MAP_TEMPLATE_H_


#include "../../include/SysMacros.h"
#include "../../include/TypeDef.h"

#include <iostream>
using namespace std;



#include "TplPool.h"

namespace Zephyr
{

#ifdef _DEBUG
#define MAX_NODE_NR (0x7FFFFFFE)
#define LIST_NODE_KEY (0x7FFFFFFF)
#else
#define MAX_NODE_NR (0xFFFFFFFE)
#define LIST_NODE_KEY (0xFFFFFFFF)
#endif

#define NEED_SIBLINGS_NR 

struct TTreeHolder
{
    void *m_pTree;
    TTreeHolder()
    {
        m_pTree = NULL;
    }
};

template<class CItem,class CKey>
class TplMultiKeyMapNode : public CItem
{
public:
    //CKey                       GetKey();
    TplMultiKeyMapNode<CItem,CKey>*    m_pLeftNode;
    TplMultiKeyMapNode<CItem,CKey>*    m_pRightNode;
    //若果是在同key列表中的话，那这个就是指向链头的.
    TplMultiKeyMapNode<CItem,CKey>*    m_pParent;
    //如果是在树中的，则指向第一个兄弟节点,不是的话，为空
    TplMultiKeyMapNode<CItem,CKey>*    m_pSiblings;
    TTreeHolder                        m_tTree; //指向保存它的tree;初始化为空
#ifdef _DEBUG
    TUInt32               m_nodeSize:31;
    TUInt32               m_isActive:1;
#else
    TUInt32               m_nodeSize; //最多0xFFFFFFFF个.
#endif
public:

#ifdef NEED_SIBLINGS_NR
  
#endif
    bool     IsTreeNode()
    {
        return (m_pSiblings != this);
    }
    //必须是树节点！返回的是兄弟数，也就是说如果同一个键值有两个实例，那么一个实例在树里，一个是兄弟，所以在树中的节点的兄弟数为1
    TUInt32 GetSiblingsNr()
    {
        if (m_pSiblings)
        {
            return m_pSiblings->m_nodeSize;
        }
        return 0;
    }
    TplMultiKeyMapNode<CItem,CKey>* GetSiblings()
    {
        if (IsTreeNode())
        {
            return m_pSiblings;
        }
        else
        {
            return m_pRightNode;
        }
    }
    
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
    void AddSiblings(TplMultiKeyMapNode<CItem,CKey>* pSiblings)
    {
        pSiblings->Init();
        pSiblings->m_pLeftNode = this;
        pSiblings->m_pRightNode = m_pSiblings;
        //pSiblings->m_nodeSize = 1;
        pSiblings->m_pParent = this; //指向树节点
        pSiblings->m_pSiblings = pSiblings;
        if (m_pSiblings)
        {
            m_pSiblings->m_pLeftNode = pSiblings;
            pSiblings->m_nodeSize = m_pSiblings->m_nodeSize + 1;
            m_pSiblings->m_nodeSize = 0; //清了.
        }
        else
        {
            pSiblings->m_nodeSize = 1;
        }
        m_pSiblings = pSiblings;
    }
    //调用者保证是属于sibling list
//     void DetachFromSibling()
//     {
//         if (m_pRightNode)
//         {
//             m_pRightNode->m_pLeftNode = m_pLeftNode;
//         }
//         //第一个兄弟节点
//         if (m_pParent == m_pLeftNode) //总是存在的
//         {
//             m_pParent->m_pSiblings = m_pRightNode;
//         }
//         else
//         {
//             m_pLeftNode->m_pRightNode = m_pRightNode;
//         }
// 
//         m_pParent = NULL;
//         m_pLeftNode = NULL;
//         m_pRightNode = NULL;
//         m_nodeSize = 0;
// 
//     }
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
public:
    
    TplMultiKeyMapNode<CItem,CKey> *GetNextSmallFirst(TplMultiKeyMapNode<CItem,CKey> *pNow)
    {
        if (!pNow)
        {
            return NULL;
        }
        TplMultiKeyMapNode<CItem,CKey> *pResult = NULL;
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

    TplMultiKeyMapNode<CItem,CKey> *FindNode(CKey& key);
    TplMultiKeyMapNode<CItem,CKey> *SafeAddNode(TplMultiKeyMapNode *pNode)
    {
            TplMultiKeyMapNode<CItem,CKey> *pResult = FindNode(pNode->GetKey());
            pNode->Init();
            pNode->m_pSiblings = NULL;
            if (pResult) //存在
            {
                pResult->AddSiblings(pNode);
                return this;
            }
            else
            {
                return AddNode(pNode);
            }
            return pResult;
    }
private:
    //if return NULL, mean this node had already existed! U can't add again!
    TplMultiKeyMapNode<CItem,CKey> *AddNode(TplMultiKeyMapNode *pNode);
public:
    //释放所有key相符合的值.如果要删除一个，那就调用下面的接口，删除指定node,注意，调用的时候必须保证这个节点属于这个树！
    TplMultiKeyMapNode<CItem,CKey> *SafeReleaseNode(TplMultiKeyMapNode<CItem,CKey> *pRelease,TInt32 &result)
    {
        TplMultiKeyMapNode<CItem,CKey> *pResult = FindNode(pRelease->GetKey());

        if (pResult)
        {
            //要释放Tree中的Node
            if (pResult == pRelease) 
            {
                if (pRelease->m_pSiblings) 
                {
                    TplMultiKeyMapNode<CItem,CKey> *pReplacement = pRelease->m_pSiblings;
                    pReplacement->m_pSiblings = pReplacement->m_pRightNode;
                    if (pReplacement->m_pRightNode)
                    {
                        pReplacement->m_pRightNode->m_nodeSize = pRelease->m_nodeSize - 1;
                        //pReplacement->m_pRightNode->m_pLeftNode = pReplacement->m_pRightNode; //不用，原来就是.
                    }
                    pReplacement->m_pLeftNode  = pRelease->m_pLeftNode;
                    pReplacement->m_pRightNode = pRelease->m_pRightNode;
                    pReplacement->m_pParent    = pRelease->m_pParent;
                    
                    pReplacement->m_nodeSize   = pRelease->m_nodeSize;
                    if (pRelease->m_pParent)
                    {
                        if(pRelease == pRelease->m_pParent->m_pLeftNode)
                        {
                            pRelease->m_pParent->m_pLeftNode = pReplacement;
                        }
                        else
                        {
                            pRelease->m_pParent->m_pRightNode = pReplacement;
                        }
                    }
                    if (pRelease->m_pLeftNode)
                    {
                        pRelease->m_pLeftNode->m_pParent = pReplacement;
                    }
                    if (pRelease->m_pRightNode)
                    {
                        pRelease->m_pRightNode->m_pParent = pReplacement;
                    }
                    pRelease->UnInit();
                    if (pRelease == this)
                    {
                        result = SUCCESS;
                        return pReplacement;
                    }
                    result = SUCCESS;
                    return this;
                }
                else //没有兄弟！！太好了
                {
                    result = SUCCESS;
                    return ReleaseNode(pRelease->GetKey());
                }
            }
            else //要删的就是兄弟节点
            {

                if (pRelease != pRelease->m_pSiblings)
                {
                    //printf("Incorrect erase !");
                    result = FAIL;
                    return this;
                }
                if (pRelease->m_pParent != pResult) //不属于这棵树！
                {
                    result = FAIL;
                    return this;
                }
                
                if (pResult->m_pSiblings == pRelease) //第一个节点
                {
                    pResult->m_pSiblings = pRelease->m_pRightNode;
                    if (pRelease->m_pRightNode)
                    {
                        pRelease->m_pRightNode->m_nodeSize = pRelease->m_nodeSize - 1;
                    }
                }
                else
                {
#ifdef _DEBUG
                    if (pRelease->m_pLeftNode) //这个应该是肯定有的！
                    {
#endif
                        pRelease->m_pLeftNode->m_pRightNode = pRelease->m_pRightNode;
                        if (pRelease->m_pRightNode)
                        {
                            pRelease->m_pRightNode->m_pLeftNode = pRelease->m_pLeftNode;
                        }
                        --pRelease->m_pParent->m_pSiblings->m_nodeSize;
#ifdef _DEBUG
                    }
#endif
                }

                pRelease->UnInit();
                result = SUCCESS;
                return this;
            }

        }
        else
        {
            result = FAIL;
            return this;
        }

        return pResult;
    }

public:
    //if the key does not exist, U wouldn't know about it.
    TplMultiKeyMapNode<CItem,CKey> *ReleaseNode(CKey& key);
    void        Init()
                {
                    m_nodeSize = 0;

                    m_pLeftNode = NULL;
                    m_pRightNode = NULL;
                    m_pParent    = NULL;
                    //m_pSiblings  = NULL;
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
                    //m_pSiblings  = NULL;
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
public:
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
    bool        NeedRearrange(TBOOL bRightNode)
                {
                    //return false;
                    TInt32 leftNodeSize = 0;
                    TInt32 rightNodeSize = 0;
                    if (m_pLeftNode)
                    {
                        leftNodeSize = m_pLeftNode->GetTreeSize();
                    }
                    if (m_pRightNode)
                    {
                        rightNodeSize = m_pRightNode->GetTreeSize();
                    }
//                     TInt32 balance = 0;
//                     TInt32 totalNum = (GetTreeSize()>>2);
                    //totalNum = totalNum>>2;
                    //totalNum += 4;
                    if (bRightNode)
                    {
                       /* balance = leftNodeSize - rightNodeSize;*/
                        if ((rightNodeSize) > (((leftNodeSize)<<1)+2))
                        {
                            return true;
                        }
                        return false;
                    }
                    else // the right node case
                    {
                        /* balance = rightNodeSize - leftNodeSize;*/
                        if ((leftNodeSize) > (((rightNodeSize)<<1)+2)) 
                        {
                            return true;
                        }
                        return false;
                        
                    }
                    /*
                    if (balance > 64)
                    {
                        return true;
                    }
                    */
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

    TplMultiKeyMapNode *MigrateRight2Left()
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
            TplMultiKeyMapNode *pNewRoot;
            //如果左子树的右子树存在
            TplMultiKeyMapNode *pRLNode = m_pRightNode->m_pLeftNode;
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
                    TplMultiKeyMapNode *pRLLNode = pRLNode->m_pLeftNode;//->m_pRightNode; //肯定是存在的
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
    TplMultiKeyMapNode *MigrateLeft2Right() 
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
            TplMultiKeyMapNode *pNewRoot;
            //如果左子树的右子树存在
            TplMultiKeyMapNode *pLRNode = m_pLeftNode->m_pRightNode;
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
                    TplMultiKeyMapNode *pLRRNode = pLRNode->m_pRightNode;//->m_pRightNode; //肯定是存在的
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
            if (m_pParent->GetKey() > GetKey())
            {
                if (m_pParent->m_pLeftNode != this)
                {
                    printf("check tree failed ,the tree parent is incorrect!\n");
                    return FAIL;
                }
            }
            else
            {
                if (m_pParent->m_pRightNode != this)
                {
                    printf("check tree failed, the tree parent is incorrect!\n");
                    return FAIL;
                }
            }
        }
        return SUCCESS;
    }
public:
    TplMultiKeyMapNode<CItem,CKey>* GetBigest()
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

    TplMultiKeyMapNode<CItem,CKey>* GetSmallest()
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

    TplMultiKeyMapNode<CItem,CKey>* GetRoot()
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
        TplMultiKeyMapNode<CItem,CKey>* m_pNow;
		//TplMultiKeyMapNode<CItem,CKey>* m_pRoot;
    public:
        Iterator()
        {
            m_pNow = NULL;
            //m_pRoot = NULL;
        }
        Iterator(TplMultiKeyMapNode<CItem,CKey>* pNow)
        {
            m_pNow = pNow;
        }
        void operator=(Iterator &rValue)
        {
            m_pNow = rValue.m_pNow;
        }
        void operator=(TplMultiKeyMapNode<CItem,CKey>* pNow)
        {
            m_pNow = pNow;
        }
        TInt32 Init(TplMultiKeyMapNode<CItem,CKey>* pNow)
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
            if (m_pNow->IsTreeNode())
            {
                if (m_pNow->m_pSiblings)
                {
                    m_pNow = m_pNow->m_pSiblings;
                    return *this;
                }
            }
            else
            {
                if (m_pNow->m_pRightNode)
                {
                    m_pNow = m_pNow->m_pRightNode;
                    return *this;
                }
                else
                {
                    m_pNow = m_pNow->m_pParent;
                }
            }
            TplMultiKeyMapNode<CItem,CKey>* pTmp = m_pNow;
            TplMultiKeyMapNode<CItem,CKey>* pLast = NULL;
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
            if (m_pNow->IsTreeNode())
            {
                if (m_pNow->m_pSiblings)
                {
                    m_pNow = m_pNow->m_pSiblings;
                    return *this;
                }
            }
            else
            {
                if (m_pNow->m_pRightNode)
                {
                    m_pNow = m_pNow->m_pRightNode;
                    return *this;
                }
                else
                {
                    m_pNow = m_pNow->m_pParent;
                }
            }

            TplMultiKeyMapNode<CItem,CKey>* pTmp = m_pNow;
            TplMultiKeyMapNode<CItem,CKey>* pLast = NULL;
            //m_pNow = NULL;
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
        operator CItem*()
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
		TplMultiKeyMapNode<CItem,CKey>* GetItem()
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
void TplMultiKeyMapNode<CItem,CKey>::Print(TInt32 etch,TInt32& count)
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
TplMultiKeyMapNode<CItem,CKey> *TplMultiKeyMapNode<CItem,CKey>::FindNode(CKey& key)
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
TplMultiKeyMapNode<CItem,CKey> *TplMultiKeyMapNode<CItem,CKey>::AddNode(TplMultiKeyMapNode *pNode)
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
    //pNode->Init();
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
            TplMultiKeyMapNode<CItem,CKey>* pNewRoot = MigrateLeft2Right();
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
            //pNewRoot->CheckTree();
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
            //pNewRoot->CheckTree();
            #endif
            return pNewRoot;
            
            /*
            TplMultiKeyMapNode<CItem,CKey>* pNewRoot = m_pLeftNode->GetBigest();

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
        TplMultiKeyMapNode<CItem,CKey>* pNewRoot = MigrateRight2Left();
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
        //pNewRoot->CheckTree();
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
        //pNewRoot->CheckTree();
        #endif
        return pNewRoot;
        /*
        TplMultiKeyMapNode<CItem,CKey>* pNewRoot = m_pRightNode->GetSmallest();
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
TplMultiKeyMapNode<CItem,CKey> *TplMultiKeyMapNode<CItem,CKey>::ReleaseNode(CKey& key)
{
    //make sure the key does existed!
    //TplMultiKeyMapNode *pResult = FindNode(key);
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
            TplMultiKeyMapNode<CItem,CKey> *pResult = m_pRightNode;
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
            TplMultiKeyMapNode<CItem,CKey> *pResult = m_pLeftNode;
            m_pLeftNode->m_pParent = m_pParent;
            UnInit();
            #ifdef _NEED_TREE_CHECK
            pResult->CheckTree();
            #endif
            return pResult;
        }
        //other cases!
        TplMultiKeyMapNode<CItem,CKey>* pNewRoot = NULL;

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
        TplMultiKeyMapNode<CItem,CKey>*   pNewRoot = this;
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
            TplMultiKeyMapNode<CItem,CKey>* pNewRoot = MigrateRight2Left();
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
        TplMultiKeyMapNode<CItem,CKey>*   pNewRoot = this;
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
            TplMultiKeyMapNode<CItem,CKey>* pNewRoot = MigrateLeft2Right();
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
class TplMultiKeyMap
{
private:

    //TInt32                   m_nMaxSize;
    //TplMultiKeyMapNode<CItem,CKey>*     m_pItem;
    CPool<TplMultiKeyMapNode<CItem,CKey> > *m_pPool;
    TplMultiKeyMapNode<CItem,CKey>*        m_pTree;
private:
    
public:
	TplMultiKeyMapNode<CItem,CKey>*     GetRoot()
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
			return m_pTree->GetTreeSize();
		}
		return 0;
	}
    
    friend class Iterator;
private:
public:
    typedef TInt32 (CItem::*_PFMSG)(TInt32);
    //Begin是有效的
    TplMultiKeyMapNode<CItem,CKey> *First()
    {
        if (m_pTree)
        {
             return m_pTree->GetSmallest();
        }
        return NULL;
    }
    //end也是有效的
    TplMultiKeyMapNode<CItem,CKey> *Last()
    {
        if (m_pTree)
        {
            return m_pTree->GetBigest();
        }
        return NULL;
    }
    void clean()
    {
        TplMultiKeyMapNode<CItem,CKey>::Iterator it = First();
        TplMultiKeyMapNode<CItem,CKey> *pNode = it.GetItem();
        int nRtn;
        while (pNode)
        {
            ++it;
            m_pTree = m_pTree->SafeReleaseNode(pNode,nRtn);
            pNode->m_tTree.m_pTree = NULL;
            
            m_pPool->ReleaseMem(pNode);

            pNode = it.GetItem();
        }
    }
    
    TplMultiKeyMap()
    {
        m_pPool = NULL;
        m_pTree = NULL;
    };
    ~TplMultiKeyMap()
    {
    }
    TInt32 Init(CPool<TplMultiKeyMapNode<CItem,CKey> > *pPool)
    {
        m_pPool = pPool;
        m_pTree = NULL;
        return SUCCESS;
    }
    void UnInit()
    {
    }
    //first get the Item
    CItem        *PrepareItem()
    {
        return m_pPool->GetMem();
    }

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

    TInt32          ReleaseItem(CItem * pItem)
    {
       TplMultiKeyMapNode<CItem,CKey> *pNode = (TplMultiKeyMapNode<CItem,CKey> *)pItem;
       if (m_pPool->ReleaseMem(pNode))
       {
           return SUCCESS;
       }
       return FAIL;    
    }

    TInt32          RemoveFromTreeItem(CItem * pItem)
    {
        TInt32 nRtn;
        if (m_pTree)
        {
            TplMultiKeyMapNode<CItem,CKey> *pNode = (TplMultiKeyMapNode<CItem,CKey>*)pItem;
            if (pNode->m_tTree.m_pTree == (void*)this)
            {
                m_pTree = m_pTree->SafeReleaseNode(pNode,nRtn);
                pNode->m_tTree.m_pTree = NULL;
            }
            else
            {
                return NOT_BELONG_TO_THIS_CAPSULA;
            }
            //pNode->UnInit();
            //pNode->m_pSiblings = NULL;
        }
        else
        {
            nRtn = NOT_BELONG_TO_THIS_CAPSULA;
        }
        return nRtn;
    }

    TInt32      AddInTree(CItem* pItem);

    TplMultiKeyMapNode<CItem,CKey> *GetItemByKey(CKey&  rKey)
    {
        if (!m_pTree)
        {
            return NULL;
        }
        return m_pTree->FindNode(rKey);
    }

    TInt32         GetFreeSize()
                {
                    return m_pPool->GetFreeNr();
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
// TInt32 TplMultiKeyMap<CItem,CKey>::Init(TInt32 size)
// {
//     if (size < 0)
//     {
//         return INPUT_PARA_ERROR;
//     }
//     // 0 ~ 1G memory was used as system area.
//     //may have warnings, but it's ok.
//     m_pTree = NULL;
// 
//     return m_tPool.InitPool(size);
// }




template<class CItem, class CKey>
TInt32 TplMultiKeyMap<CItem,CKey>::AddInTree(CItem* pItem)
{
    if ((NULL == pItem))
    {
        return NULL_POINTER;
    }

    TplMultiKeyMapNode<CItem,CKey> *pNew = static_cast< TplMultiKeyMapNode<CItem,CKey>* >(pItem);
//     TInt32 offset = ((char*)pNew - (char*)m_pItem);
// 
// 
//     if ((offset < 0)||(0 != (offset % sizeof( TplMultiKeyMapNode<CItem,CKey>* ))))
//     {
//         return NOT_BELONG_TO_THIS_CAPSULA;
//     }
    //pNew->Init();
    if (pNew->m_tTree.m_pTree)
    {
        return ALREADY_IN_TREE;
    }
    pNew->m_tTree.m_pTree = (void*)this;
    if (m_pTree)
    {
        m_pTree = m_pTree->SafeAddNode(pNew);
    }
    else
    {
        pNew->Init();
        pNew->m_pSiblings = NULL;
        m_pTree = pNew;
    }
    
    return SUCCESS;
}

}
#endif

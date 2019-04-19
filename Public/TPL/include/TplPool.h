#ifndef __ZEPHYR_PUBLIC_TPL_TPL_POOL_H__
#define __ZEPHYR_PUBLIC_TPL_TPL_POOL_H__
#include "../../include/TypeDef.h"
#include "../../include/SysMacros.h"
#include "TplList.h"
namespace Zephyr
{
//2级内存池，每个内存对象都保存自己属于哪个内存块CMemBlock，然后释放自己，如果CMemBlock是额外内存，把所有内存都回收了，则释放整个内存块
//所有额外的内存都会自动回收.并且占有非常少的操作
template <class Mem>
class CPool
{
private:
    class CMemBlock;
    class CPoolMem : public Mem
    {
    public:
#ifdef MEM_FIRST   
        union //存一个就行了
        {
#endif
            CPoolMem    *m_pMemPoolNext;
            CMemBlock   *m_pBelongsTo;
#ifdef MEM_FIRST  
        };
#endif
public:
// #ifdef MEM_FIRST  
//         void Init(CListNode<CMemBlock>  *pBelongs2)
//         {
//             m_pBelongsTo       = pBelongs2;
//         }
//         
// #else
//         void Init(CPoolMem *pNext,CListNode<CMemBlock>  *pBelongs2)
//         {
//             m_pMemPoolNext     = pNext;
//             m_pBelongsTo       = pBelongs2;
//         }
// #endif
        void ReleaseSelf()
        {
            m_pBelongsTo->m_pContainer->ReleaseMem(this);
        }
    };
    class CMemBlock
    {
        //DECLARE_CLASS_LIST(CMemBlock)
    private:
        CPoolMem *m_pPools;
        CPoolMem *m_pFree;
        int       m_nSize;
        int       m_nFree;
        CPool    *m_pContainer;
        CList<CMemBlock> *m_pBelongsTo;
        bool Belongs2Me(CPoolMem *pPools)
        {
            if ((pPools >= m_pPools) && (pPools < m_pPools+m_nSize))
            {
                return true;
            }
            return false;
        }
    public:
        friend CPool;
        int GetFreeNr()
        {
            return m_nFree;
        }
        CMemBlock()
        {
            m_nFree = 0;
            m_nSize = 0;
            m_pPools = NULL;
            m_pFree  = NULL;
            m_pContainer = NULL;
            m_pBelongsTo = NULL;
        }
        ~CMemBlock()
        {
            if (m_pPools)
            {
                delete []m_pPools;
            }
        }
        int Init(int nSize,CList<CMemBlock> *pBlongsTo,CPool *pContainer)
        {
            try
            {
                m_pPools = new CPoolMem[nSize];
            }
            catch (...)
            {
            	
            }
            if (m_pPools)
            {
                for (int i=0;i<nSize;++i)
                {
#ifndef MEM_FIRST  
                    m_pPools[i].m_pBelongsTo = this;
#endif
                    m_pPools[i].m_pMemPoolNext = m_pPools + i + 1;
                }
                m_pPools[nSize-1].m_pMemPoolNext = NULL;
                m_pFree = m_pPools;
                m_nSize = nSize;
                m_nFree = nSize;
                m_pBelongsTo = pBlongsTo;
                m_pContainer = pContainer;
                return nSize;
            }
            else
            {
                return OUT_OF_MEM;
            }
        }
        bool IsEmpty()
        {
            return (!m_nFree);
        }
        bool CanRecycle()
        {
            return (m_nFree == m_nSize);
        }
        CPoolMem *GetMem()
        {
            if (m_pFree)
            {
                CPoolMem *pRtn = m_pFree;
                m_pFree = m_pFree->m_pMemPoolNext;
                pRtn->m_pMemPoolNext = NULL;
                --m_nFree;
#ifdef MEM_FIRST  
                pRtn->m_pBelongsTo = this;
#endif
                return pRtn;
            }
            return NULL;
        }
        bool      ReleaseMem(CPoolMem *pRel)
        {
#ifdef _DEBUG
            if(Belongs2Me(pRel))
            {
#endif
                pRel->m_pMemPoolNext = m_pFree;
                m_pFree = pRel;
                ++m_nFree;
                return true;
#ifdef _DEBUG
            }
            return false;
#endif
        }
    };
    
    CList<CMemBlock> m_tUsingMemBlocks;
    CList<CMemBlock> m_tFullMemBlocks;
    CListNode<CMemBlock> *m_pMainBlock;
    int        m_nInitSize;
    int        m_nFreeNr;
public:
    ~CPool()
    {
        Final();
    }
    int GetFreeNr()
    {
        return m_nFreeNr;
    }
    int InitPool(unsigned int size)
    {
        CListNode<CMemBlock> *pBlock = NULL;
        try 
        {
            pBlock = new CListNode<CMemBlock>;
        }
        catch(...)
        {

        }
        if (!pBlock)
        {
            return OUT_OF_MEM;
        }
        int nRet = pBlock->Init(size,&m_tUsingMemBlocks,this);
        if (nRet < SUCCESS)
        {
            return nRet;
        }
        m_tUsingMemBlocks.push_back(pBlock);
        m_pMainBlock = pBlock;
        m_nInitSize = size;
        m_nFreeNr   = size;
        return SUCCESS;
    }
    void Final()
    {
        CListNode<CMemBlock> *p = m_tUsingMemBlocks.pop_front();
        while (p)
        {
            CListNode<CMemBlock> *pDel = p;
            p = m_tUsingMemBlocks.pop_front();
            delete pDel;
        }
    }
    bool IsNotMainBlock(CMemBlock* pBlock)
    {
        if (pBlock != m_pMainBlock)
        {
            return true;
        }
        return false;
    }
    void OnBlockUsed(CListNode<CMemBlock> *pBlock)
    {
        if (pBlock->IsEmpty()) //空了!
        {
            pBlock->m_pBelongsTo->Detach(pBlock);
            m_tFullMemBlocks.push_front(pBlock);
            pBlock->m_pBelongsTo = &m_tFullMemBlocks;
        }
    }
    //之时是否要delete 这个块所属的Blocks
    void OnBlockRecycled(CListNode<CMemBlock> *pBlock)
    {
        if (IsNotMainBlock(pBlock)) //非主块，放后面
        {
           if (pBlock->CanRecycle())
           {
                pBlock->m_pBelongsTo->Detach(pBlock);
                m_tUsingMemBlocks.push_back(pBlock);
                pBlock->m_pBelongsTo = &m_tUsingMemBlocks;
           }
           else
           {
                if (pBlock->m_pBelongsTo == &m_tFullMemBlocks)
                {
                    pBlock->m_pBelongsTo->Detach(pBlock);
                    if (m_pMainBlock == m_tUsingMemBlocks.header())
                    {
                        m_pMainBlock->InsertNode(pBlock);
                    }
                    else
                    {
                        m_tUsingMemBlocks.push_front(pBlock);
                    }
                    pBlock->m_pBelongsTo = &m_tUsingMemBlocks;
                }
               //else 什么也不动
           }
        }
        else //主块放前面，每次用header的，帮助非主块释放
        {
            if (pBlock->m_pBelongsTo != &m_tUsingMemBlocks)
            {
                pBlock->m_pBelongsTo->Detach(pBlock);
                pBlock->m_pBelongsTo = & m_tUsingMemBlocks;
                m_tUsingMemBlocks.push_front(pBlock);
            }
        }
        if (m_nFreeNr > (m_nInitSize))
        {
            CListNode<CMemBlock> *pRear = m_tUsingMemBlocks.rear();
            if ((pRear)&&(IsNotMainBlock(pRear))&&(pRear->CanRecycle()))
            {
                CListNode<CMemBlock> *pDel = pRear;
                m_tUsingMemBlocks.Detach(pRear);
                pRear = m_tUsingMemBlocks.rear();
                m_nFreeNr -= pDel->m_nSize;
                delete pDel;
            }
        }
    }
    Mem *GetMem()
    {
        CListNode<CMemBlock> *pBlock = m_tUsingMemBlocks.header();
        if (pBlock)
        {
            CPoolMem *pRtn = pBlock->GetMem();
            if (pRtn) //这个是必须的
            {
                OnBlockUsed(pBlock);
                --m_nFreeNr;
                return pRtn;
            }
            else //呃回收一下？
            {
                OnBlockUsed(pBlock); 
            }
        }
        try
        {
            pBlock = new CListNode<CMemBlock>();
        }
        catch (...)
        {
        }
        if (pBlock)
        {
            if (pBlock->Init(((m_nInitSize>>1)+1),&m_tUsingMemBlocks,this) < SUCCESS)
            {
                delete pBlock;
                return NULL;
            }
            else
            {
                m_tUsingMemBlocks.push_back(pBlock);
            }
        }
        if (pBlock)
        {
            //m_tUsingMemBlocks.push_front(pBlock);
            m_nFreeNr += pBlock->GetFreeNr();
            //肯定成功
            --m_nFreeNr;
            return pBlock->GetMem();
        }
        return NULL;
    }
    bool ReleaseMem(Mem *pMem)
    {
        CPoolMem *p = (CPoolMem*)pMem;
        CListNode<CMemBlock>  *pBlock = (CListNode<CMemBlock>  *)p->m_pBelongsTo;
        if(pBlock->ReleaseMem(p))
        {
            ++m_nFreeNr;
        }
        OnBlockRecycled(pBlock);
        return true;
    }
};

template <class ITEM_CLASS>
class CArrayPoolNode : public ITEM_CLASS
{
public:
    CList<CArrayPoolNode> *m_pListBelongsTo;

    void InitInPoolList(CList<CArrayPoolNode> *pList)
    {
        m_pListBelongsTo = pList;
    }
};

template <class ITEM_CLASS>
class TplArrayPool
{
private:
    
    TUInt32 m_nMaxSize;
    TUInt32 m_nUsedSize;
    
    CListNode<CArrayPoolNode<ITEM_CLASS> > *m_pPool;
public:
    //只用来判断是否为空闲
    CList<CArrayPoolNode<ITEM_CLASS> >   m_tFree;
public:
   TInt32 InitPool(TUInt32 nSize)
   {
       try
       {
           m_pPool = new CListNode<CArrayPoolNode<ITEM_CLASS> >[nSize];
       }
       catch (...)
       {
           return OUT_OF_MEM;
       }
       for (TUInt32 i =0;i<nSize;++i)
       {
           m_pPool[i].InitInPoolList(&m_tFree);
           m_tFree.push_back(&m_pPool[i]);
       }
       m_nMaxSize = nSize;
       m_nUsedSize = 0;
       return SUCCESS;
   }
   TUInt32 GetMaxSize()
   {
       return m_nMaxSize;
   }
    CListNode<CArrayPoolNode<ITEM_CLASS> > *GetMem()
    {
         CListNode<CArrayPoolNode<ITEM_CLASS> > *p = m_tFree.pop_front();
         if (p)
         {
             p->m_pListBelongsTo = NULL;
             ++m_nUsedSize;
         }
         return p;
    }
    CListNode<CArrayPoolNode<ITEM_CLASS> > *FindMem(TUInt32 nIdx)
    {
        return m_pPool + (nIdx % m_nMaxSize);
    }
    ITEM_CLASS *FindUsedMem(TUInt32 nIdx)
    {
        if (m_pPool[nIdx%m_nMaxSize].m_pListBelongsTo != &m_tFree)
        {
            return m_pPool[nIdx%m_nMaxSize];
        }
        return NULL;
    }
    void ReleaseMem(CListNode<CArrayPoolNode<ITEM_CLASS> > *pNode)
    {
        if (pNode->m_pListBelongsTo)
        {
            pNode->m_pListBelongsTo->Detach(pNode);
            pNode->m_pListBelongsTo = NULL;
        }
        m_tFree.push_back(pNode);
        --m_nUsedSize;
        pNode->m_pListBelongsTo = &m_tFree;
    }
};

template <class ITEM_CLASS>
class ItemClassPool 
{
private:
    TUInt32             m_maxItemNum;
    TUInt32             m_freeItemNum;
    ITEM_CLASS          *m_pItemPool;
    ITEM_CLASS          *m_pFreeHeader;
    ITEM_CLASS          *m_pFreeRear;
    ITEM_CLASS          *m_pUsed;
public:
    ItemClassPool()
    {
        m_maxItemNum = 0;
        m_freeItemNum = 0;       
        m_pItemPool = NULL;
        m_pFreeHeader = NULL;
        m_pFreeRear   = NULL;
        m_pUsed = NULL;
    }
    ~ItemClassPool()
    {
        if (m_pItemPool)
        {
            delete [] m_pItemPool;
        }
    }
    TInt32              Init(TInt32 nrOfMaxItem)
    {
        try
        {
            m_pItemPool = new ITEM_CLASS[nrOfMaxItem];
        }
        catch (...)
        {
        }
        if (!m_pItemPool)
        {
            return OUT_OF_MEM;
        }
        m_pFreeHeader =m_pItemPool;
        m_pFreeRear = (m_pItemPool);
        m_pFreeHeader->Init();
        for (TInt32 i=1;i<nrOfMaxItem;i++)
        {
            m_pFreeRear->AttachList((m_pItemPool+i));
            m_pFreeRear = (m_pItemPool+i);
            m_pFreeRear->Init();
        }
        m_maxItemNum = nrOfMaxItem;
        m_freeItemNum = nrOfMaxItem;
        return SUCCESS;
    }
    ITEM_CLASS*        GetItem()
    {
        ITEM_CLASS *pResult = NULL;
        if (m_pFreeHeader)
        {
            pResult = m_pFreeHeader;
            m_pFreeHeader = m_pFreeHeader->GetNext();
            if (!m_pFreeHeader)
            {
                m_pFreeRear = NULL;
            }
            pResult->Detach();
            pResult->AttachList(m_pUsed);
            m_pUsed = pResult;
            pResult->OnInit();
            --m_freeItemNum;
        }
        return pResult;
    }
    void                ReleaseItem(ITEM_CLASS *pItem)
    {
        //从used列表里删除
        if (!pItem->IsActived())
        {
            return;
        }
        pItem->OnFinal();
        //不负责从原队列里删除，交给使用者.
        if (m_pUsed == pItem)
        {
            m_pUsed = pItem->GetNext();
        }
        pItem->Detach();
        if(m_pFreeRear)
        {
            m_pFreeRear->AttachList(pItem);
            m_pFreeRear = pItem;
        }
        else
        {
            m_pFreeHeader = pItem;
            m_pFreeRear   = pItem;
        }
        ++m_freeItemNum;
        //pItem->Attach(m_pFree);
    }
    ITEM_CLASS*        GetConectionByIdx(TUInt32 idx)
    {
        if (idx < m_maxItemNum)
        {
            return m_pItemPool + idx;
        }
        return NULL;
    }
    TUInt32 GetSize()
    {
        return m_maxItemNum;
    }
};


}

#define DECLARE_STATIC_CLASS_POOL(CLASS_NAME) \
    static CPool<CLASS_NAME>  sm_tPool; \
    static TInt32 InitPool(TInt32 nPoolSize);

#define IMPLEMENT_STATIC_CLASS_POOL(CLASS_NAME) \
    CPool<CLASS_NAME>  CLASS_NAME::sm_tPool; \
    TInt32 CLASS_NAME::InitPool(TInt32 nPoolSize) \
    { \
         return sm_tPool.InitPool(nPoolSize); \
    }

#define INIT_STATIC_POOL(CLASS_NAME,N_SIZE) CLASS_NAME::InitPool(N_SIZE);

#define CREATE_FROM_STATIC_POOL(CLASS_NAME) \
    CLASS_NAME::sm_tPool.GetMem();

#define RELEASE_INTO_STATIC_POOL(CLASS_NAME,PT) \
CLASS_NAME::sm_tPool.ReleaseMem(PT); \
PT = NULL;


#endif

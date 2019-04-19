#include "../include/HeaderFile.h" 
#include "../include/interfaceElement.h"
#include "../include/nameSpace.h"
#include<iostream>
namespace Zephyr
{



IMPLEMENT_STATIC_CLASS_POOL(CHeaderFile);

string CHeaderFile::sm_szServiceName;
const char *CHeaderFile::m_pFileName=NULL;
CHeaderFile::CHeaderFile()
{
    m_ppElements = NULL;
    m_ppWords = NULL;
    m_nNrOfWords = 0;
    m_nNrOfElements = 0;
    m_pszFile = NULL;
    m_nElmentType = raw_header_file_type;
    
    INIT_STATIC_POOL(CBaseElement,256);

    if (NULL == sm_pBaseElements)
    {
        sm_pBaseElements = new TplMap<TplPtPack<CBaseElement,string >,string>;
        CPool<TplNode<TplPtPack<CBaseElement,string >,string> > *pPool = new CPool<TplNode<TplPtPack<CBaseElement,string >,string> >;
        pPool->InitPool(100);
        sm_pBaseElements->Init(pPool);
        AddBaseType("TInt32",raw_TInt32_type);
        AddBaseType("TInt16",raw_TInt16_type);
        AddBaseType("TInt8",raw_TInt8_type);
        AddBaseType("TInt64",raw_TInt64_type);
        AddBaseType("TFloat",raw_TFloat_type);
        AddBaseType("TDouble",raw_TDouble_type);
        AddBaseType("TBool",raw_TBool_type);
        AddBaseType("TChar",raw_TChar_type);
        AddBaseType("TUInt32",raw_TUInt32_type);
        AddBaseType("TUInt16",raw_TUInt16_type);
        AddBaseType("TUInt8",raw_TUInt8_type);
        AddBaseType("TUInt64",raw_TUInt64_type);
    }
    if (NULL == sm_pBaseKeyWords)
    {
        CPool<TplNode<TplPtPack<CBaseElement,string >,string> > *pPool = new CPool<TplNode<TplPtPack<CBaseElement,string >,string> >;
        pPool->InitPool(100);
        sm_pBaseKeyWords = new TplMap<TplPtPack<CBaseElement,string >,string>;
        sm_pBaseKeyWords->Init(pPool);

         AddKeyWords("class",key_class);
         AddKeyWords("namespace", key_namespace);
         AddKeyWords("struct",key_struct);
         AddKeyWords("static",key_static);
         AddKeyWords("const",key_const );
         AddKeyWords("volatile",key_volatile     );
         AddKeyWords("mutable",key_mutable      );
         AddKeyWords("public",key_public       );
         AddKeyWords("protected",key_protected    );
         AddKeyWords("private",key_private      );
         AddKeyWords("include",key_include      );
         AddKeyWords("pragma",key_pragma       );
         AddKeyWords("define",key_nr_define       );
         AddKeyWords("ifdef",key_nr_ifdef        );
         AddKeyWords("ifndef",key_nr_ifndef       );
         AddKeyWords("endif",key_nr_endif        );
         AddKeyWords("while",key_while        );
         AddKeyWords("if",key_if           );
         AddKeyWords("else",key_else         );
         AddKeyWords("for",key_for          );
         AddKeyWords("break",key_break        );
         AddKeyWords("continue",key_continue     );
         AddKeyWords("goto",key_goto         );
         AddKeyWords("switch",key_switch       );
         AddKeyWords("extern",key_extern       );
         AddKeyWords("virtual",key_virtual);
         AddKeyWords("typedef",key_type_define);
    }
}

CHeaderFile::~CHeaderFile()
{

}

//CPool<CBaseElement> *CHeaderFile::sm_pKeyWordsPool = NULL;

TInt32 CHeaderFile::GeneratorIdl(const char *pFileName,const char *pPath)
{
    int nRet = ReadFile(pFileName);
    if (nRet < SUCCESS)
    {
        printf("Can not open file:%s",pFileName);
        return nRet;
    }
    m_pFileName = pFileName;
    DividIntoWords();
    
    RemoveAllCommentsAndMakeConstStr();
    RemoveAllType(divider_type);
    RemoveAllNumLine();
    RemoveAllType(enter_type);
//     for (int i=0;i<m_nNrOfWords;++i)
//     {
//         if (enter_type == m_pWordsTypes[i])
//         {
//             printf("\n");
//         }
//         else
//         {
//             printf("@%s$",m_ppWords[i]);
//         }
//     }
    nRet = Process(m_ppWords,m_pWordsTypes,0,m_nNrOfWords);
    if (nRet == m_nNrOfWords)
    {
        printf("\nParser Txt success, we got whole syntax tree!\n");
    }
    else
    {
        printf("\nSyntax incorrect! at %d\n",nRet);
        printf("press any key to continue");
        char c;
        std::cin>>c;
    }
    printf("\nInput Service Name :");
    std::cin>>sm_szServiceName;
    printf("\nInput Interface Id Start Num:");
    std::cin>>CBaseElement::sm_nInterfaceIdBegin;
    for (int i=0;i<sm_szServiceName.size();++i)
    {
        sm_szServiceName[i] = toupper(sm_szServiceName[i]);
    }
    GenerateStub(pPath);
    GenerateSkeleton(pPath);
    GenerateMethodId(pPath);
    return SUCCESS;
}

    //读取文件,调用者保证pFileName的可用性
TInt32 CHeaderFile::ReadFile(const char *pFileName)
{
    FILE *pFile = fopen(pFileName,"r");
    if (pFile)
    {
        fseek(pFile,0,SEEK_END);
        m_nFileLength = ftell(pFile);
        if (m_nFileLength)
        {
            m_pszFile = new char[m_nFileLength];
            rewind(pFile);

            int result = fread (m_pszFile,1,m_nFileLength,pFile);
            if (result < m_nFileLength) 
            {
                m_nFileLength = result;
            }
        }
        fclose (pFile);
        return SUCCESS;
    }
    return FAIL;
}


void CHeaderFile::RemoveAllType(EnType enType)
{
    int nCopy2 = 0;
    for (int i=0;i<m_nNrOfWords;++i)
    {
        if (enType == m_pWordsTypes[i])
        {
            continue;
        }
        else
        {
            m_ppWords[nCopy2] = m_ppWords[i];
            m_pWordsTypes[nCopy2] = m_pWordsTypes[i];
            ++nCopy2;
        }
    }
    m_nNrOfWords = nCopy2;
}

TInt32 CHeaderFile::Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles)
{
    //最后再处理一遍
    int nNr=nProcess2;
    while(nNr < nTotalEles )
    {
        CBaseElement *pBase = IsKeyWords(ppElements[nNr]);
        if (pBase)
        {
            switch (pBase->m_nElmentType)
            {
            case key_class:
                {
                    CInterfaceElement *pInterface = CREATE_FROM_STATIC_POOL(CInterfaceElement);
                    if (pInterface)
                    {
                        int nRet = pInterface->Process(ppElements,pTypes,(nNr),nTotalEles);
                        if (nRet <= 0)
                        {
                            return nRet;
                        }
                        nNr += nRet;
                        AddChildElement(pInterface);
                    }
                }
                break;
            case key_namespace:
                {
                    CNamespace *pInterface = CREATE_FROM_STATIC_POOL(CNamespace);
                    if (pInterface)
                    {
                        ++nNr;
                        int nRet = pInterface->Process(ppElements,pTypes,(nNr),nTotalEles);
                        if (nRet <= 0)
                        {
                            return nRet;
                        }
                        else
                        {
                            AddChildElement(pInterface);
                        }
                        nNr += nRet;
                    }
                }
                break;
            case key_struct:
                {

                }
                break;
            }
        }
        else
        {
            //process a line!
        }
    }
    
    
    
    //IgnorType(m_ppWords,m_pWordsTypes,0,m_nNrOfWords,divider_type);
    return (nNr - nProcess2);
}

void CHeaderFile::RemoveAllNumLine() //删除所有'#'
{
    enum EnTxtState
    {
        en_normal   = 0,
        en_1st_num_mark_found ,
        en_include_line ,
        en_ifdef_line,
        en_ifndef_line,
        en_else_line,
        en_endif_line,
    };
    EnTxtState enState= en_normal;
    int nCopy2 = 0;
    for (int i=0;i<m_nNrOfWords;++i)
    {
        switch(enState)
        {
        case en_normal:
            {
                if (operator_type==m_pWordsTypes[i])
                {
                    if ('#' == m_ppWords[i][0])
                    {
                        enState = en_1st_num_mark_found;
                        break;
                    }
                }
                m_ppWords[nCopy2] = m_ppWords[i];
                m_pWordsTypes[nCopy2] = m_pWordsTypes[i];
                ++nCopy2;
            }
            break;
        case en_1st_num_mark_found:
            {
                if (alphabet_type == m_pWordsTypes[i])
                {
                    CBaseElement *pEle = IsKeyWords(m_ppWords[i]);
                    if (pEle)
                    {
                        switch (pEle->m_nElmentType)
                        {
                        case key_nr_define:
                        case key_nr_ifdef:
                        case key_nr_ifndef:
                        case key_nr_endif:
                        case key_else:
                        case key_include:
                            {
                                enState = en_ifndef_line;
                            }
                            break;
                        }
                        if (enState !=en_1st_num_mark_found)
                        {
                            break;
                        }
                    }
                }
                printf("Error, incorrect format after #");
            }
            break;
        case en_ifndef_line:
            {
                if (enter_type == m_pWordsTypes[i])
                {
                    enState = en_normal;
                }
            }
            break;
        }
    }
    m_nNrOfWords = nCopy2;
}



void CHeaderFile::RemoveAllCommentsAndMakeConstStr()
{
    enum EnTxtState
    {
        en_normal   = 0,
        en_1st_slash = 1, //第一个斜杠
        en_2nd_slash = 2, //第二个斜杠
        en_1st_start = 3, //一个斜杠后跟着的的第一个*
        en_2nd_start = 4, //第二个*
        en_1st_sigle_quote_mark = 5,
        en_1st_sigle_quote_mark_back_slash_found  = 6,
        en_1st_sigle_char_found=7,
        en_1st_quote_mark = 8,
    };
    EnTxtState enState = en_normal;
    int nCopy2 = 0;
    int nPushed = 0;
    for (int i=0;i<m_nNrOfWords;++i)
    {
        char *pElements = m_ppWords[i];
        EnType enType = m_pWordsTypes[i];
        switch (enState)
        {
        case en_normal:
            {
                
                //查找第一个 /
                if (slash_mark_type == enType)
                {
                    ++nPushed;
                    enState = en_1st_slash;
                }
                else if(single_quotes_mark_type == enType)
                {
                    ++nPushed;
                    enState = en_1st_sigle_quote_mark;
                }
                else if(quotes_mark_type == enType)
                {
                    ++nPushed;
                    enState = en_1st_quote_mark;
                }
                else
                {
                    m_ppWords[nCopy2] = pElements;
                    m_pWordsTypes[nCopy2] = enType;
                    ++nCopy2;
                }
            }
            break;
        case en_1st_slash:
            {
                if (slash_mark_type == m_pWordsTypes[i])
                {
                    ++nPushed;
                    enState = en_2nd_slash;
                }
                else if (star_mark_type == m_pWordsTypes[i])
                {
                    ++nPushed;
                    enState = en_1st_start;
                }
                else
                {
                    enState = en_normal;
                    //把push的偿还
                    m_ppWords[nCopy2] = m_ppWords[i-1];
                    m_pWordsTypes[nCopy2] = m_pWordsTypes[i-1];
                    ++nCopy2;
                    m_ppWords[nCopy2] = pElements;
                    m_pWordsTypes[nCopy2] = enType;
                    ++nCopy2;
                    nPushed = 0;
                }
            }
            break;
        case en_2nd_slash:
            {
                if (enter_type == m_pWordsTypes[i])
                {
                    //MakeOneWords(m_ppWords,i-nPushed,nPushed+1);
                    //m_ppWords[nCopy2] = m_ppWords[i-nPushed];
                    //m_pWordsTypes[nCopy2] = m_pWordsTypes[i-nPushed];
                    //++nCopy2;
                    nPushed = 0;
                    enState =  en_normal;
                }
                else
                {
                    ++nPushed;
                }
            }
            break;
        case en_1st_start:
            {
                if (star_mark_type == m_pWordsTypes[i])
                {
                    ++nPushed;
                    enState = en_2nd_start;
                }
                else
                {
                    ++nPushed;
                }
            }
            break;
        case en_2nd_start:
            {
                if (slash_mark_type == m_pWordsTypes[i])
                {
                    //MakeOneWords(m_ppWords,i-nPushed,nPushed+1);
                    //m_ppWords[nCopy2] = m_ppWords[i-nPushed];
                    //m_pWordsTypes[nCopy2] = m_pWordsTypes[i-nPushed];
                    //++nCopy2;
                    nPushed = 0;
                    enState = en_normal;
                }
                else if (star_mark_type == m_pWordsTypes[i])
                {
                    ++nPushed;
                    enState = en_2nd_start;
                }
                else
                {
                    ++nPushed;
                    enState = en_1st_start;
                }
            }
            break;
        case en_1st_sigle_quote_mark:
            {
                if (back_slash_mark_type == m_pWordsTypes[i])
                {
                    ++nPushed;
                    enState = en_1st_sigle_quote_mark_back_slash_found;
                }
                else 
                {
                    ++nPushed;
                    enState = en_1st_sigle_char_found;
                }
            }
            break;            
        case en_1st_sigle_quote_mark_back_slash_found:
            {
                //合并
                ++nPushed;
                enState = en_1st_sigle_char_found;
            }
            break;
        case en_1st_sigle_char_found:
            {
                if (single_quotes_mark_type == m_pWordsTypes[i])
                {
                    MakeOneWords(m_ppWords,i-nPushed,nPushed+1);
                    m_ppWords[nCopy2] = m_ppWords[i-nPushed];
                    m_pWordsTypes[nCopy2] = m_pWordsTypes[i-nPushed];
                    ++nCopy2;
                    nPushed = 0;
                    enState = en_normal;
                }
                else 
                {
                    printf("Error,incorrect format!");
                    return;
                }
            }
            break;
        case en_1st_quote_mark :
            {
                if (quotes_mark_type == m_pWordsTypes[i])
                {
                    MakeOneWords(m_ppWords,i-nPushed,nPushed+1);
                    m_ppWords[nCopy2] = m_ppWords[i-nPushed];
                    m_pWordsTypes[nCopy2] = m_pWordsTypes[i-nPushed];
                    ++nCopy2;
                    nPushed = 0;
                    enState = en_normal;
                }
                else
                {
                    ++nPushed;
                }
            }
            break;
        }
    }
    m_nNrOfWords = nCopy2;
}



    //分词
TInt32 CHeaderFile::DividIntoWords()
{
    
    EnType cTypes[256];
    char c = 0;
    for (int i=0;i<256;++i)
    {
        if (IsSemicolon(c))
        {
            cTypes[i] = semicolon_type;
        }
        else if (IsBlanket1(c))
        {
            cTypes[i] = blanket_type_1;
        }
        else if (IsBlanket2(c))
        {
            cTypes[i] = blanket_type_2;
        }
        else if (IsEnter(c))
        {
            cTypes[i] = enter_type;
        }
        else if (IsDivider(c))
        {
            cTypes[i] = divider_type;
        }
        else if (IsOperator(c))
        {
            cTypes[i] = operator_type;
        }
        else if (IsAlphabet(c))
        {
            cTypes[i] = alphabet_type;
        }
        else if (IsNum(c))
        {
            cTypes[i] = num_type;
        }
        else if (IsSlashMark(c))
        {
            cTypes[i] = slash_mark_type;
        }
        else if (IsStarMark(c))
        {
            cTypes[i] = star_mark_type;
        }
        else if (IsSingleQuote(c))
        {
            cTypes[i] = single_quotes_mark_type;
        }
        else if (IsQuote(c))
        {
            cTypes[i] = quotes_mark_type;
        }
        else if (IsBackSlash(c))
        {
            cTypes[i] = back_slash_mark_type;
        }
        else if (IsBiggerMark(c))
        {
            cTypes[i] = bigger_mark_type;
        }
        else if (IsSmallerMark(c))
        {
            cTypes[i] = smaller_mark_type;
        }
        else 
        {
            cTypes[i] = not_acceptable_type;
        }
        ++c;
    }
    //空格 0 符号 1 字母 2 数字 3 
    EnType nLastType = divider_type;
    int nLastWordIdx = 0;
    m_pszFileInPiece = new char[m_nFileLength*2];
    memset(m_pszFileInPiece,0,(m_nFileLength*2));
    m_ppWords = new char*[m_nFileLength/2];
    m_pWordsTypes = new EnType[m_nFileLength/2];
    memset(m_ppWords,0,(sizeof(char*)*(m_nFileLength/2)));
    m_nNrOfWords = 0;
    char *pRead2 = m_pszFileInPiece;
    nLastType =(EnType)cTypes[m_pszFile[0]];

    for (int i = 1;i<m_nFileLength;++i)
    {
        unsigned char *pCur = (unsigned char*)&m_pszFile[i];
        switch (nLastType)
        {
        case enter_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case enter_type:
                    {
                        
                    }
                    break;
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case divider_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        //继续
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case operator_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case operator_type:
                    {
                        //继续
                        switch (*pCur)
                        {
                        case '&':
                        case '|':
                        case '-':
                        case '+':
                        case '=':
                            {
                                if (i == nLastWordIdx+1)
                                {
                                    unsigned char *pPre = pCur-1;
                                    if (*pCur == *pPre)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case alphabet_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case enter_type:
                case semicolon_type:
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        //continue
                    }
                    break;
                case num_type:
                    {
                        //continue 
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)pCur[1])>0x80) //是个中文字符？！
                                {
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case num_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case enter_type:
                case semicolon_type:
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        //继续
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        //继续
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)(*(pCur+1)))>0x80) //是个中文字符？！
                                {
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return  -1;
                    }
                }
            }
            break;
        case slash_mark_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = divider_type;
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case star_mark_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = divider_type;
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case single_quotes_mark_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = divider_type;
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case quotes_mark_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = divider_type;
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case back_slash_mark_type:
            {
                switch (cTypes[(unsigned int)(*pCur)])
                {
                case smaller_mark_type:
                case bigger_mark_type:
                case blanket_type_1:
                case blanket_type_2:
                case semicolon_type:
                case enter_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = cTypes[(unsigned int)(*pCur)];
                    }
                    break;
                case divider_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = divider_type;
                    }
                    break;
                case operator_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;

                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = operator_type;
                    }
                    break;
                case alphabet_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = alphabet_type;
                    }
                    break;
                case num_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = num_type;
                    }
                    break;
                case slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = slash_mark_type;
                    }
                    break;
                case back_slash_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = back_slash_mark_type;
                    }
                    break;
                case star_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = star_mark_type;
                    }
                    break;
                case single_quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = single_quotes_mark_type;
                    }
                    break;
                case quotes_mark_type:
                    {
                        memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                        pRead2[(i-nLastWordIdx)] = 0;
                        m_ppWords[m_nNrOfWords] = pRead2;
                        m_pWordsTypes[m_nNrOfWords] = nLastType;
                        ++m_nNrOfWords;
                        pRead2 += ((i-nLastWordIdx)+1);
                        nLastWordIdx = i;
                        nLastType = quotes_mark_type;
                    }
                    break;
                default:
                    {
                        if (((unsigned int)(*pCur))>0x80)
                        {
                            if (i < (m_nFileLength-1))
                            {
                                if (((unsigned int)m_pszFile[i+1])>0x80) //是个中文字符？！
                                {
                                    memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                                    pRead2[(i-nLastWordIdx)] = 0;
                                    m_ppWords[m_nNrOfWords] = pRead2;
                                    m_pWordsTypes[m_nNrOfWords] = nLastType;
                                    ++m_nNrOfWords;
                                    pRead2 += ((i-nLastWordIdx)+1);
                                    nLastWordIdx = i;
                                    ++i; //再跳过一个字符
                                    nLastType = alphabet_type;
                                    break;
                                }
                            }
                        }
                        printf("find unacceptable char:%d",((int)m_pszFile[i]));
                        return -1;
                    }
                }
            }
            break;
        case blanket_type_1:
        case blanket_type_2:
        case bigger_mark_type:
        case smaller_mark_type:
        case semicolon_type:
            {
                memcpy(pRead2,m_pszFile+nLastWordIdx,i-nLastWordIdx);
                pRead2[(i-nLastWordIdx)] = 0;
                m_ppWords[m_nNrOfWords] = pRead2;

                m_pWordsTypes[m_nNrOfWords] = nLastType;
                ++m_nNrOfWords;
                pRead2 += ((i-nLastWordIdx)+1);
                nLastWordIdx = i;
                nLastType = cTypes[(unsigned int)(*pCur)]; 
            }
            break;
        default:
            {
                if (((unsigned int)(*pCur))>0x80)
                {
                    nLastType = alphabet_type;
                    break;
                }
                printf("find unacceptable char:%d",((int)m_pszFile[i]));
                return -1;
            }
        }
    }
    //RemoveType(divider_type);
//     for (int i=0;i<m_nNrOfWords;++i)
//     {
//         if (enter_type == m_pWordsTypes[i])
//         {
//             printf("\n");
//         }
//         else
//         {
//             printf("@%s$",m_ppWords[i]);
//         }
//     }
    return SUCCESS;
}

TBOOL  CHeaderFile::IsDivider(char c)
{
    switch(c)
    {
    case ' ':
        {
        }
        break;
    case '\t':
        {
        }
        break;
    default:
        {
            return FALSE;
        }
    }
    return TRUE;
}

TBOOL CHeaderFile::IsEnter(char c )
{
    if ('\n' == c)
    {
        return TRUE;
    }
    return FALSE;
}

TBOOL CHeaderFile::IsOperator(char c)
{

    switch(c)
    {
    case '&':
        {

        }
        break;
    case '+':
        {

        }
        break;
    case '-':
        {

        }
        break;
    case '=':
        {
        }
        break;
    case '{':
        {
        }
        break;
    case '}':
        {
            
        }
        break;
    case '!':
        {
        }
        break;
    case '[':
        {
        }
        break;
    case ']':
        {
            
        }
        break;
    case '~':
        {

        }
        break;
    case '#':
        {

        }
        break;
    case ',':
        {

        }
        break;
    case '|':
        {

        }
        break;
    case '?':
        {

        }
        break;
    case '.':
        {

        }
        break;
    case ':':
        {

        }
        break;
    case '%':
        {

        }
        break;
    case '^':
        {

        }
        break;
    default:
        {
            return FALSE;
        }
    }
    return TRUE;
}

TBOOL  CHeaderFile::IsAlphabet(char c)
{
    if ((c>='a')&&(c<='z'))
    {
        return TRUE;
    }
    if ((c>='A')&&(c<='Z'))
    {
        return TRUE;
    }
    if (c == '_')
    {
        return TRUE;
    }
    return FALSE;
}
TBOOL  CHeaderFile::IsNum(char c)
{
    if ((c>='0')&&(c<='9'))
    {
        return TRUE;
    }
    return FALSE;
}

TInt32 CHeaderFile::GeneratorIdlCode()
{
    return SUCCESS;
}

TInt32 CHeaderFile::GenerateSkeleton(const char *pPath)
{
    int n = m_tChilds.size();
    int nTotalSize = 0;
    for(int i=0;i<n;++i)
    {
        CBaseElement *p = m_tChilds[i].m_pPt;
        switch (p->m_nElmentType)
        {
        case raw_interface_type:
        case raw_namespace_type:
        case raw_method_type:
            {
                int nRet = p->GenerateSkeleton(pPath);
                if (nRet < SUCCESS)
                {
                    nTotalSize += nRet;
                }
            }
            break;
        default:
            {
                
            }
        }
    }
    return nTotalSize;
}

TInt32 CHeaderFile::GenerateStub(const char *pPath)
{
    int n = m_tChilds.size();
    int nTotalSize = 0;
    for(int i=0;i<n;++i)
    {
        CBaseElement *p = m_tChilds[i].m_pPt;
        switch (p->m_nElmentType)
        {
        case raw_interface_type:
        case raw_namespace_type:
        case raw_method_type:
            {
                int nRet = p->GenerateStub(pPath);
                if (nRet < SUCCESS)
                {
                    nTotalSize += nRet;
                }
            }
            break;
        default:
            {

            }
        }
    }
    return nTotalSize;
}

}

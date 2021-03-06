#pragma once
#include "libRaftExt.h"
#include "RaftStorage.h"
namespace rocksdb
{
    class DB;
};
namespace raftpb
{
    class Snapshot;
};

///\brief 采用RocksDB实现日志的持久化存储
class LIBRAFTEXT_API CRocksDbStorage : public CRaftStorage
{
public:
    ///\brief 构造函数
    ///\param pLogger 日志输出
    CRocksDbStorage(CLogger * pLogger, CRaftSerializer *pRaftSerializer = NULL);

    ///\brief 析构函数
    virtual ~CRocksDbStorage(void);

    ///\brief 初始化
    ///\param strDbPath 保存数据的目录
    ///\return 成功标志 0 成功，其他失败
    virtual int Init(std::string &strDbPath);
    
    ///\brief 释放资源
    virtual void Uninit(void);
    
    ///\brief 取得第一个索引号
    ///\param u64Index 返回的索引号
    ///\return 成功标志 0 成功，其他失败
    virtual int FirstIndex(uint64_t &u64Index);

    ///\brief 取得最后一个索引号
    ///\param u64Index 返回的索引号
    ///\return 成功标志 0 成功，其他失败
    virtual int LastIndex(uint64_t &u64Index);

    ///\brief 设置提交索引号
    ///\param u64Committed 提交索引号
    ///\return 成功标志 0 成功，其他失败
    virtual int SetCommitted(uint64_t u64Committed);

    ///\brief 设置应用索引号
    ///\param u64tApplied 提交应用号
    ///\return 成功标志 0 成功，其他失败
    virtual int SetApplied(uint64_t u64tApplied);

    ///\brief 取得提交索引号
    ///\param u64Committed 返回的提交索引号
    ///\return 成功标志 0 成功，其他失败
    virtual int GetCommitted(uint64_t &u64Committed);

    ///\brief 取得应用索引号
    ///\param u64Applied 返回提交应用号
    ///\return 成功标志 0 成功，其他失败
    virtual int GetApplied(uint64_t &u64Applied);

    ///\brief 取得日志中对应索引号的任期号
    ///\param u64Index 索引号
    ///\param u64Term 取得的任期号
    ///\return 成功标志 0 成功，其他失败
    virtual int Term(uint64_t u64Index, uint64_t &u64Term);

    ///\brief 追加日志到存储中
    ///\param entries 日志数组
    ///\return 成功标志 0 成功，其他失败
    virtual int Append(const EntryVec& entries);

    ///\brief 读取日志
    ///\param u64Low 起始索引号
    ///\param u64High 终止索引号
    ///\param u64MaxSize 最大容量，字节数
    ///\param entries 返回的日志数组
    ///\return 成功标志 0 成功，其他失败
    virtual int Entries(uint64_t u64Low, uint64_t u64High, uint64_t u64MaxSize, vector<CRaftEntry> &entries);

    virtual int InitialState(CHardState &stateHard, CConfState &stateConfig);
    
    virtual int SetHardState(const CHardState &stateHard);
    
    ///\brief 创建快照
    ///\param u64Index 日志索引号
    ///\param pConfState 如果不为空，则替代快照的节点ID集合
    ///\param strData  快照的数据
    ///\param pSnapshot 如果不为空，返回当前的快照.
    ///\return 成功标志 OK 成功；其他失败
    virtual int CreateSnapshot(uint64_t i, CConfState *cs, const string& data, CSnapshot *ss);

    ///\brief 取得当前快照
    ///\param snapshot 返回的快照对象
    ///\return 成功标志 OK 成功；其他失败
    virtual int GetSnapshot(CSnapshot & snapshot);
protected:

    ///\brief 通过判断迭代器是否有效判断数据库是否为空
    ///\return true 没有数据 ；false 有数据
    bool IsEmpty(void);
protected:
    rocksdb::DB * m_pStocksDB;      ///< RocksDB对象
    CSnapshot * m_pSnapShot; ///< 快照
    CLogger * m_pLogger;            ///< 用于输出日志
    CHardState m_stateHard;
    uint64_t m_u64FirstIndex; ///< 第一个索引号
    uint64_t m_u64LastIndex ; ///< 最后一个索引号
};

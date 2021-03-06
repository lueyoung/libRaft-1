#pragma once
#include "libRaftCore.h"
#include "RaftDef.h"

//使用内存数组维护所有的更新日志项。
//对于Leader节点来说，它维护了客户端的更新请求对应的日志项；
//对于Follower节点而言，它维护的是Leader节点复制的日志项。
//无论是Leader还是Follower节点，日志项首先都会被存储在unstable结构，
//然后再由其内部状态机将unstable维护的日志项交给上层应用，
//由应用负责将这些日志项进行持久化并转发至系统其它节点。
//这也是为什么它被称为unstable的原因：
///\attention 在unstable中的日志项都是不安全的，尚未持久化存储，可能会因意外而丢失。
// unstable.entries[i] has raft log position i+unstable.offset.
// Note that unstable.offset may be less than the highest log
// position in storage; this means that the next write to storage
// might need to truncate the log before persisting unstable.entries.
class LIBRAFTCORE_API CUnstableLog
{
public:
    CUnstableLog(CLogger *pLogger = NULL) : m_pSnapshot(NULL), m_pLogger(pLogger)
    {
        m_u64Offset = 0;
    }

    ~CUnstableLog(void)
    {
        if (NULL != m_pSnapshot)
        {
            delete m_pSnapshot;
            m_pSnapshot = NULL;
        }
    }
    void TruncateAndAppend(const EntryVec& entries);

    ///\brief 尝试取得第一条日志的索引号
    ///\param u64Last 取得的第一条日志的索引号
    ///\return 成功标志 true 成功; false 失败
    ///\attention 如果有快照，才会取得成功，否则从持久化日志存储中取得
    bool MaybeFirstIndex(uint64_t &u64First);

    ///\brief 尝试取得最后一条日志的索引号
    ///\param u64Last 取得的最后一条日志的索引号
    ///\return 成功标志 true 成功; false 失败
    ///\attention 如果有至少1条日志或者有快照，才会取得成功
    bool MaybeLastIndex(uint64_t &u64Last);

    ///\brief 尝试按日志索引号取得日志的任期号
    ///\param u64Index 日志索引号
    ///\param u64Term 取得日志的任期号
    ///\return 成功标志 true 成功; false 失败
    ///\attention 如果有日志索引号在Offset和最后一条非持久日志返回内，或者恰好是快照对应的索引号，才会取得成功
    bool MaybeTerm(uint64_t u64Index, uint64_t  &u64Term);

    ///\brief 部分日志持久化，从非持久化管理中移出
    ///\param u64Index 日志号
    ///\param u64Term 日志对应的任期号
    ///\attention 如果索引号非法或者任期号不对应，则不执行日志移出操作
    void StableTo(uint64_t u64Index, uint64_t u64Term);

    void StableSnapTo(uint64_t u64Index);

    ///\brief 本身数据复位，接受快照对象
    ///\param snapshot 快照对象
    void Restore(const CSnapshot& snapshot);

    ///\brief 按索引号范围（闭区间）读取一个片段
    ///\param u64Low 起始索引号
    ///\param u64High 终止索引号
    ///\param vecEntries 返回的日志数组
    ///\attention 索引号范围必须是合法的范围，否则会调用日志的Fatalf接口
    void Slice(uint64_t u64Low, uint64_t u64High, EntryVec &vecEntries);
protected:
    ///\brief 断言索引号范围的正确性
    ///\param u64Low 起始索引号
    ///\param u64High 终止索引号
    ///\attention 要求u64Low 小于u64High，u64Low大于等于m_u64Offset并且u64High小于等于上限
    void AssertCheckOutOfBounds(uint64_t u64Low, uint64_t u64High);

public:
    CSnapshot* m_pSnapshot; ///< 快照
    EntryVec m_vecEntries;  ///< 写入内存但未持久化的日志
    uint64_t m_u64Offset;   ///< 第一个日志记录的索引号偏移
    CLogger *m_pLogger;     ///< 输出日志
};

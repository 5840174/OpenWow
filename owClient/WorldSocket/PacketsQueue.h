#pragma once

class ZN_API CPacketsQueue
{
public:
	inline void Add(const std::shared_ptr<CServerPacket>& item)
	{
		std::lock_guard<std::mutex> lock(m_QueueLock);
		m_Queue.push_back(item);
	}

	inline std::shared_ptr<CServerPacket> GetNextItem()
	{
		std::shared_ptr<CServerPacket> packetPtr;

		{
			std::lock_guard<std::mutex> lock(m_QueueLock);
			if (m_Queue.empty())
				return nullptr;
			packetPtr = m_Queue.front();
			m_Queue.pop_front();
		}

		return packetPtr;
	}

	inline bool IsEmpty()
	{
		std::lock_guard<std::mutex> lock(m_QueueLock);
		return m_Queue.empty();
	}

	inline void Clear()
	{
		std::lock_guard<std::mutex> lock(m_QueueLock);
		m_Queue.clear();
	}

private:
	std::mutex m_QueueLock;
	std::list<std::shared_ptr<CServerPacket>> m_Queue;
};
#pragma once
#include <memory>
#include <mutex>
#define DYNAMIC template<class T>
namespace liao::PrimedDB
{ 
	DYNAMIC
	class Memory
	{
	private:
		std::shared_ptr<T> m_mem;
		mutable std::mutex m_mutex;
		unsigned int m_size;
		using lock = std::unique_lock<std::mutex>;
		using mutexLock = std::lock_guard<std::mutex>;

		void allocate(unsigned int size)
		{
			if (m_mem == nullptr || (m_size < size && m_mem.use_count() <= 1))
			{
				m_mem = std::shared_ptr<T>(new T[size]);
				m_size = size;
			}

		}

	public:
		//Default constructor which set size to 0 and mem to null;
		Memory()
		{
            m_mem = std::shared_ptr<T>();
            m_size = 0;
		}
		//Use given memory and size to contruct Memory. 
		//The pointer given to constructor should not be use anymore.
		Memory(T* mem, unsigned int size)
		{
			set(mem, size);
		}
		//Copy constructor
		Memory(const Memory& other)
		{
			m_mem = std::shared_ptr<T>(new T[other.m_size]);
            memcpy(m_mem.get(), other.m_mem.get(), other.m_size);
            m_size = other.m_size;
		}
		//Delete memory contains in this object if it is not null.
		void clear()
		{
			mutexLock lock(m_mutex);
			if (m_mem.use_count() <= 1)
			{
				m_mem.reset();
				m_mem = nullptr;
				m_size = 0;
			}
		}
		//Check if memory is null
		bool isNull() const
		{
			mutexLock lock{ m_mutex };
            return m_mem.get() == nullptr;
		}
		//Set memory and size for this object and delete original memory.
		bool set(T* mem, unsigned int size)
		{
			if (mem == nullptr || m_mem.use_count() > 1)
				return false;
			mutexLock lock(m_mutex);
			m_mem = std::shared_ptr<T>(mem);
			m_size = size;
			return true;
		}
		//Set memory and size for this object and delete original memory.
		bool set(std::shared_ptr<T>& other, unsigned int size)
		{
			lock lock(m_mutex);
            m_mem = std::move(other);
			other = nullptr;
            m_size = size;
		}
		//Set memory and size for this object and delete original memory.
		bool set(Memory& other)
		{
			lock lock(m_mutex);
			lock.lock();
			if (m_mem.use_count() <= 1)
			{
				m_mem = std::move(other.m_mem);
				m_size = other.m_size;
				lock.unlock();
				other.clear();
				return true;
			}
			return false;
		}
		//Move ownership of current momery in object to other object. Set ptr to null after the operation.
		bool move(std::shared_ptr<T>& container)
		{
			{
				mutexLock lock(m_mutex);
				if (m_mem == nullptr)
				{
					return false;
				}
				container = std::move(m_mem);
			}
			clear();
			return true;
		}
		//Move ownership of current momery in object to other object. Set ptr to null after the operation.
		bool move(Memory& container)
		{
			{
				mutexLock lock(m_mutex);
				if (m_mem == nullptr)
					return false;
				container.set(*this);
			}
			clear();
			return true;
		}
		//Make a copy of current memory and store it in other object.
		bool duplicateTo(Memory& other)
		{
			lock lock(m_mutex);
			lock.lock();
			if(m_mem == nullptr)
                return false;
			other.m_mem = std::shared_ptr<T>(new T[m_size]);
            std::memcpy(other.m_mem.get(), m_mem.get(), m_size);
            other.m_size = m_size;
			lock.unlock();
            return true;
		}
		//Make a copy of current memory and store it in other object.
		bool duplicateTo(std::shared_ptr<T>& container)
		{
			lock lock(m_mutex);
			lock.lock();
			if(m_mem == nullptr)
                return false;
            container = std::shared_ptr<T>(new T[m_size]);
            std::memcpy(container.get(), m_mem.get(), m_size);
			lock.unlock();
            return true;
		}
		void modify(const T* mem, unsigned int size)
		{
			allocate(size);
			std::memcpy(m_mem.get(), mem, std::min(m_size, size));
		}
		void modify(const std::shared_ptr<T> mem, unsigned int size)
		{
			allocate(size);
			std::memcpy(m_mem.get(), mem.get(), std::min(m_size,size));
		}
		void modify(const Memory<T>& mem)
		{
			allocate(mem.m_size);
			std::memcpy(m_mem.get(), mem.m_mem.get(), std::min(m_size, mem.m_size));
		}
		//Get how many references of current memory
		unsigned int count() const
		{
			mutexLock lock(m_mutex);
            return m_mem.use_count();
		}

		//Compare the content in memory with given memory.
		bool compare(const Memory& other)const
		{
			mutexLock lock(m_mutex);
			int size = std::max(m_size, other.m_size);
            std::unique_ptr<T> mem = std::make_unique<T>();
			mem.reset(new T[size]);
			if (m_size < other.m_size)
			{
                std::memcpy(mem.get(), m_mem.get(), m_size);
			}
			else
			{
                std::memcpy(mem.get(), other.m_mem.get(), other.m_size);
			}
            return std::memcmp(m_mem.get(), mem.get(), size) == 0;
		}
		//Compare the content in memory with given memory.
		//This method need caller to ensure given memory is long enough.
		bool compare(const T* memory) const
		{
			mutexLock lock(m_mutex);
            return std::memcmp(m_mem.get(), memory, m_size) == 0;
		}
		//Compare the content in memory with given memory.
		//This method need caller to ensure given memory is long enough.
		bool compare(const std::shared_ptr<T>& other) const
		{
			mutexLock lock(m_mutex);
            return std::memcmp(m_mem.get(), other.get(), m_size) == 0;
		}
		std::shared_ptr<T> reference()
		{
			mutexLock lock(m_mutex);
            return m_mem;
		}
		//Get the memory stored in current object. 
		const std::shared_ptr<T>& get() const
		{
			mutexLock lock(m_mutex);
			return m_mem;
		}
		//equavilent to compare()
		bool operator== (const Memory& other) const
		{
			return compare(other);
		}
		//equavilent to compare()
		bool operator== (const T* other) const
		{
			return compare(other);
		}
		//equavilent to compare()
		bool operator== (const std::shared_ptr<T>& other) const
		{
			return compare(other);
		}
		//Opposite to isNull()
		operator bool() const
		{
			return !isNull();
		}

		//Destructor
		~Memory()
		{
			clear();
		}
	};
}

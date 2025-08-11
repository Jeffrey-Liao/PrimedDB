#include "Memory.h"
namespace liao::PrimedDB 
{ 
	//Default constructor which set size to 0 and mem to null;
	Memory::Memory()
        : m_size(0), m_mem(nullptr), m_count(new int)
	{}
	//Use given memory and size to contruct Memory. 
	//The pointer given to constructor should not be use anymore.
	Memory::Memory(char* mem, unsigned int size)
	{
		set(mem, size);
	}
	//Copy constructor
	Memory::Memory(const Memory& other)
	{
		if (!other.m_mem)
		{
			this->m_mem = new char[other.m_size];
			std::memcpy(this->m_mem, other.m_mem, other.m_size);
			this->m_size = other.m_size;
		}
	}
	//Delete memory contains in this object if it is not null.
	void Memory::clear()
	{
		if (m_mem)
		{
			delete[] m_mem;
		}
		m_size = 0;
		*m_count = 0;
        m_mem = nullptr;
	}
	//Check if memory is null
	bool Memory::isNull() const
	{
		return m_mem == nullptr;
	}
	//Set memory and size for this object and delete original memory.
	void Memory::set(char* mem, unsigned int size)
	{
        clear();
		if (m_count == nullptr)
		{
			m_count = new unsigned int;
			*m_count =  0;
		}

        m_mem = mem;
        m_size = size;
	}
	//Set memory and size for this object and delete original memory.
	void Memory::set(std::shared_ptr<char>& other, unsigned int size)
	{
        clear();
		if (other.use_count() == 1)
		{
			m_mem = other.get();
            other.reset();
            m_size = size;
		}
	}
	//Set memory and size for this object and delete original memory.
	void Memory::set(Memory& other)
	{
		clear();
		m_mem = other.m_mem;
        m_size = other.m_size;
        other.clear();
	}
	//Move ownership of current momery in object to other object. Set ptr to null after the operation.
	void Memory::move(std::shared_ptr<char>& container)
	{

	}
	//Move ownership of current momery in object to other object. Set ptr to null after the operation.
	void move(Memory& container);
	//Make a copy of current memory and store it in other object.
	void duplicateTo(Memory& other);
	//Make a copy of current memory and store it in other object.
	void duplicateTo(std::shared_ptr<char>& container);

	//Compare the content in memory with given memory.
	void compare(const Memory& other)const;
	//Compare the content in memory with given memory.
	//This method need caller to ensure given memory is long enough.
	void compare(const char* memory) const;
	//Compare the content in memory with given memory.
	//This method need caller to ensure given memory is long enough.
	void compare(const std::shared_ptr<char>& other) const;

	//Get the memory stored in current object. 
	const char* get() const;
	//equavilent to compare()
	bool operator== (const Memory& other) const;
	//equavilent to compare()
	bool operator== (const char* other) const;
	//equavilent to compare()
	bool operator== (const std::shared_ptr<char>& other) const;
	//equavilent to isNull()
	operator bool() const;

	//Destructor
	Memory::~Memory()
	{
		clear();
		delete m_count;
	}
}
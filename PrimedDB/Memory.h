#pragma once
#include <memory>
#include <mutex>
namespace liao::PrimedDB
{ 
	class Memory
	{
	private:
		char* m_mem;
		unsigned int* m_count;
		std::mutex m_mutex;
		unsigned int m_size;
	public:
		//Default constructor which set size to 0 and mem to null;
        Memory();
		//Use given memory and size to contruct Memory. 
		//The pointer given to constructor should not be use anymore.
        Memory(char* mem, unsigned int size);
		//Copy constructor
		Memory(const Memory& other);
		//Delete memory contains in this object if it is not null.
		void clear();
		//Check if memory is null
		bool isNull() const;
		//Set memory and size for this object and delete original memory.
        void set(char* mem, unsigned int size);
		//Set memory and size for this object and delete original memory.
		void set(std::shared_ptr<char>& other, unsigned int size);
		//Set memory and size for this object and delete original memory.
		void set(Memory& other);
		//Move ownership of current momery in object to other object. Set ptr to null after the operation.
		void move(std::shared_ptr<char>& container);
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
        ~Memory();
	};
}

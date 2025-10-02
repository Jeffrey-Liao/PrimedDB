#pragma once
#include "Global.h"
#include "Setting.h"

namespace liao::PrimedDB
{
	class Block
	{
		std::string m_id;
		struct OwnerInfo
		{
			std::shared_ptr<std::string> m_ownerName;
			std::shared_ptr<std::vector<bool>> m_available;
			unsigned m_beginLine;

			OwnerInfo() = default;
			OwnerInfo(OwnerInfo&&);
		} m_owner;
		//memory
		char* m_memory;
		//size of how many valid bytes in memory
		unsigned int m_size;
		//size of how many record in memory
		unsigned m_recordSize;
		mutable ShareMutex m_mutex;
		void allocate(const char* source = nullptr, unsigned size = 0);
		void deallocate();

	public:
		Block();
		Block(Block&)=delete;
		Block(Block&&) noexcept;
		void assign(std::shared_ptr<std::string>&, std::shared_ptr<std::vector<bool>>&, unsigned);
		//check two Blocks are same object or not
		bool same(const Block&) const;
		//compare content in memory
		bool equal(const Block&) const;
		void resize();
		//return success(true) fail(false)
		unsigned int write(std::string&,unsigned byteSize);
		unsigned int write(char*, unsigned size, unsigned byteSize);
		void write(std::shared_ptr<std::fstream> file, unsigned byteSize);
		bool empty() const;
		unsigned int size() const;
		unsigned reocrdSize() const;
		const char* reference() const;
		void remove(unsigned index) const;
		bool insert(unsigned byteSize, char* memory);
		void modify(unsigned index, unsigned byteSize, char* memory,int size);
		const char* get(unsigned index, unsigned byteSize)const;
		UCharPtr release();
		//==same
		bool operator==(const Block&)  const;
		Block& operator=(Block&&) noexcept;
		~Block();
	};
}

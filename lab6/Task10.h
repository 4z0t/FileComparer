#pragma once
#include<iostream>
#include <fstream>
#include <mutex>
#include<thread>
#include <future>
#include <chrono>
#include <memory>
#include <filesystem>
#include<algorithm>





class Task10
{

	typedef std::filesystem::path fspath;
	typedef std::filesystem::directory_entry fs_directory_entry;
	typedef std::shared_ptr<fs_directory_entry> fs_directory_entry_ptr;
	typedef std::pair< fs_directory_entry_ptr, fs_directory_entry_ptr> fs_directory_entry_ptr_pair;
	std::vector<fs_directory_entry_ptr> files;


public:
	Task10() :files()
	{

	}

	void start(const fspath& check_path)
	{

		for (auto& p : std::filesystem::recursive_directory_iterator(check_path))
		{

			//std::cout << "found " << p.path() << std::endl;
			bool is_found = false;
			if (p.is_regular_file())
				for (auto path_p = this->files.begin(); path_p != this->files.end(); path_p++)
				{
					if (this->renamer(**path_p, p))
					{
						is_found = true;
						this->files.erase(path_p);
						break;
					}
				}
			if (!is_found)
				this->files.push_back(std::make_shared<fs_directory_entry>(p));
		}
	}
private:
	inline static bool name_compare(const fs_directory_entry& de1, const fs_directory_entry& de2)
	{
		return de1.path().stem() == de2.path().stem();
	}
	inline static bool shallow_compare(const fs_directory_entry& de1, const fs_directory_entry& de2)
	{
		return de1.file_size() == de2.file_size();
	}

	static fspath add_to_filename(const fspath& path, const std::string& s)
	{
		return path.parent_path() / (path.stem().string() + s + path.extension().string());
	}

	bool renamer(const fs_directory_entry& de1, const fs_directory_entry& de2)
	{
		if (name_compare(de1, de2))
		{
			if (shallow_compare(de1, de2))
			{
				if (deep_compare(de1, de2))
				{
					std::cout << "File removed:\n " <<
						'\t' << de2.path() << '\n' <<
						"\tsame files:\n"<<
						'\t' << de1.path() << '\n' <<
						'\t' << de2.path() << '\n' <<
						std::endl;

				}
				else
				{
					std::size_t s1 = de1.file_size();
					std::size_t s2 = de2.file_size();
					fspath path1 = add_to_filename(de1.path(), ('_' + std::string("v1,") + std::to_string(s1)));
					fspath path2 = add_to_filename(de2.path(), ('_' + std::string("v2,") + std::to_string(s1)));

					std::cout << "Files renamed:\n " <<
						'\t' << de1.path() << '\n' <<
						'\t' << de2.path() << '\n' <<
						"\tto\n" <<
						'\t' << path1 << '\n' <<
						'\t' << path2 << '\n' <<
						std::endl;

				}

			}
			else
			{
				std::size_t s1 = de1.file_size();
				std::size_t s2 = de2.file_size();
				fspath path1 = add_to_filename(de1.path(), ('_' + std::to_string(s1)));
				fspath path2 = add_to_filename(de2.path(), ('_' + std::to_string(s2)));
				/*fspath path1 = de1.path().parent_path() / de1.path().stem() / ('_' + std::to_string(s1)) / de1.path().extension();
				fspath path2 = de2.path().parent_path() / de2.path().stem() / ('_' + std::to_string(s2)) / de2.path().extension();*/
				std::cout << "Files renamed:\n " <<
					'\t' << de1.path() << '\n' <<
					'\t' << de2.path() << '\n' <<
					"\tto\n" <<
					'\t' << path1 << '\n' <<
					'\t' << path2 << '\n' <<
					std::endl;

			}
			return true;
		}
		return false;
	}

	static bool deep_compare(const fs_directory_entry& de1, const fs_directory_entry& de2)
	{
		std::ifstream f1(de1.path(), std::ios::binary);
		std::ifstream f2(de2.path(), std::ios::binary);
		if (f1.is_open() && f2.is_open())
		{
			/*f1.open(de1.path());
			f2.open(de2.path());*/
			while (!f1.eof() && !f2.eof())
				if (f1.get() != f2.get())
				{

					f1.close();
					f2.close();
					return false;
				}

			bool res = (f1.eof() == f2.eof());
			f1.close();
			f2.close();
			return res;

		}
		return false;

	}




};


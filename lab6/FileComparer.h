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




class FileComparer
{
	typedef std::filesystem::path fspath;
	typedef std::filesystem::directory_entry fs_directory_entry;
	typedef std::shared_ptr<fs_directory_entry> fs_directory_entry_ptr;
	typedef std::pair< fs_directory_entry_ptr, fs_directory_entry_ptr> fs_directory_entry_ptr_pair;

	std::chrono::time_point<std::chrono::system_clock> start_time;

	std::vector<
		std::unique_ptr<
		std::pair<
		std::shared_ptr<
		fs_directory_entry_ptr_pair
		>,
		std::future<bool>//result of comparison
		>
		>
	> results;
	std::vector<fs_directory_entry_ptr> files;
	std::mutex mut;
	bool is_done;
public:
	std::mutex output;
	FileComparer() :files(), results(), is_done(false)
	{


	}
	void start(const fspath& check_path, const fspath& dump_path)
	{
		this->start_time = std::chrono::system_clock::now();
		std::thread go_through_dirs_thread(&FileComparer::go_through_dirs, this, std::ref(check_path));
		std::thread distributor_thread(&FileComparer::distributor, this);
		std::thread dumper_thread(&FileComparer::dumper, this, std::ref(dump_path));
		go_through_dirs_thread.join();
		distributor_thread.join();
		dumper_thread.join();

		std::cout << "Files scaned: " << this->files.size() << std::endl;
		/*for (auto& p : this->results)
		{
			if (p->second.get())
			{
				std::cout << "Found equal files:\n" << '\t' << p->first->first->path().relative_path() << "\n\t" << p->first->second->path().relative_path() << std::endl;
			}
		}*/

	}
private:
	inline static bool shallow_compare(const fs_directory_entry& de1, const fs_directory_entry& de2)
	{
		return de1.file_size() == de2.file_size();
	}
	//TODO:
	// use hashing  instead
	//md5
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


	void dumper(const fspath& path)
	{
		std::ofstream file(path, std::ios::ate);
		if (!file.is_open()) throw std::exception("cant open file for dump!");
		while (true)
		{
			std::lock_guard lock(this->mut);
			while (true) {
				if (this->results.size()) {
					auto res = std::move(*this->results.back());
					std::async(std::launch::async, [&res, this, &file] {
						auto resl = std::move(res);
						if (resl.second.get())
						{
							std::lock_guard lock(this->output);
							file << "Found equal files:\n" << '\t'
								<< resl.first->first->path().lexically_normal().generic_string()
								<< "\n\t"
								<< resl.first->second->path().lexically_normal().generic_string()
								<< std::endl;
						}
						});
					this->results.pop_back();
				}
				else {
					if (this->is_done)
					{
						file << "Files scaned: " << this->files.size() << std::endl;
						file << "Elapsed time: "
							<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - this->start_time).count()
							<< " ms" << std::endl;
						file.close();
						return;
					}
					break;
				}
			}
		}

	}

	void go_through_dirs(const fspath& path)
	{

		for (auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			//std::cout << "found " << p.path() << std::endl;
			std::lock_guard lock(this->mut);
			if (p.is_regular_file())
				this->files.push_back(std::make_shared<fs_directory_entry>(p));
		}
		this->is_done = true;
	}

	//distributes files to be compared
	void distributor()
	{
		//std::vector <std::shared_ptr<fs_directory_entry_ptr_pair>> file_pairs;
		std::size_t last_end = this->files.size();

		while (!this->is_done)
		{

			std::lock_guard lock(this->mut);
			if (last_end != this->files.size()) {
				for (auto path_p1 = this->files.begin(); path_p1 != this->files.end(); path_p1++)
				{

					for (auto path_p2 = (this->files.begin() + last_end > path_p1) ? last_end + this->files.begin() : path_p1 + 1; path_p2 != this->files.end(); path_p2++)
					{
						//if (!this->is_in_pairs(file_pairs, *path_p1, *path_p2))
						if (shallow_compare(**path_p1, **path_p2))
						{
							fs_directory_entry_ptr_pair files_pair;


							files_pair.first = *path_p1;
							files_pair.second = *path_p2;
							std::pair<
								std::shared_ptr<
								fs_directory_entry_ptr_pair
								>,
								std::future<bool>//result of comparison
							> res;
							res.first = std::make_shared<fs_directory_entry_ptr_pair>(files_pair);
							res.second = std::async(std::launch::async, &FileComparer::deep_compare, std::ref(**path_p1), std::ref(**path_p2));
							this->results.push_back(std::make_unique<std::pair<
								std::shared_ptr <
								fs_directory_entry_ptr_pair>,
								std::future<bool>//result of comparison
								>>());
							*this->results.back() = std::move(res);
							//file_pairs.push_back(this->results.back()->first);
						}
					}
				}
			}
			last_end = this->files.size();
		}


	}


};


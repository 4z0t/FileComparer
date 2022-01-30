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
#include <regex>


class Task9
{
	const std::string CHAR_SEQ = "([^-]+)";
	typedef std::filesystem::path fspath;
	typedef std::filesystem::directory_entry fs_directory_entry;
	typedef std::shared_ptr<fs_directory_entry> fs_directory_entry_ptr;
	typedef std::pair< fs_directory_entry_ptr, fs_directory_entry_ptr> fs_directory_entry_ptr_pair;



public:
	Task9()
	{

	}

	void start(const fspath& check_path, const std::string& pattern)
	{
		

		std::cout << "applied pattern: " << pattern << std::endl;
		std::regex pattern_regex(CHAR_SEQ + '-' + '(' + pattern + ')' + CHAR_SEQ + '-' + CHAR_SEQ);
		for (auto& p : std::filesystem::recursive_directory_iterator(check_path))
		{
			if (p.is_regular_file())
			{
				//std::cout << "Found: " << p.path() << std::endl;
				if (std::regex_match(p.path().stem().string(), pattern_regex))
				{
					std::string s = p.path().stem().string();
					//s = pattern + "_" + std::regex_replace(s, std::regex(pattern), "");
					s = std::regex_replace(s, std::regex(pattern_regex), "$2_$1-$3-$4");
					std::cout << "File renamed:\n " <<
						'\t' << p.path() << '\n' <<
						"\tto\n" <<
						'\t' << p.path().parent_path() / (s + p.path().extension().string()) << '\n' <<
						std::endl;
				}
			}
		}
	}





};


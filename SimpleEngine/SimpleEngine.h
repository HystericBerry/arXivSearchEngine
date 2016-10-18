#ifndef SIMPLE_ENGINE_H
#define SIMPLE_ENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/chrono.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "SimpleEngine.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
//#include "Time.h"
#include <list>

#include "boost/filesystem/path.hpp"

class SimpleEngine {

public:
	/*
	* Takes a Porter Stemmer and a directory full of .json files to populate an inverted index that
	* is allocated on the heap. This method feeds the text file into a stringstream and passes the
	* stringstream into a boost, json property tree. The tokens are individually transformed to
	* lowercase and stemmed before being put into the inverted index.
	*/
	static void populateIndex(const boost::filesystem::path &dir, PorterStemmer &stemmer, InvertedIndex *& idx,
		std::unordered_map<unsigned int, std::string> *idTable) {

		boost::chrono::high_resolution_clock::time_point totalStart, totalFinish, start, finish;
		totalStart = boost::chrono::high_resolution_clock::now();
		double elapsedTime = 0.0, stemTime = 0.0, fileReadTime = 0.0, treeTime = 0.0, lowerTime = 0.0, stringTime = 0.0;

		std::unordered_map<std::string, std::string> cache;
		boost::filesystem::directory_iterator it(dir), eod;
		std::vector<std::string> mPathList;
		getPathNames(dir, mPathList);
		std::sort(mPathList.begin(), mPathList.end());

		int i = 0;
		for (auto p : mPathList) {
			//++i;
			//if (i == 100 || i == 5000 || i == 10000 || i == 15000) 
			//std::cout << "Processing Article" << i << ".json" << std::endl;

			std::cout << "Processing Article (" << (i++) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;

			// reads json file into stringstream and populates a json tree
			std::ifstream file(p);
			std::stringstream ss;
			ss << file.rdbuf();
			file.close();

			boost::property_tree::ptree pt;
			boost::property_tree::read_json(ss, pt);
			boost::filesystem::path dir(p);
			(*idTable)[i] = dir.stem().string();

			//std::cout << "json to map...\n";
			// iterate through .json tree
			BOOST_FOREACH(boost::property_tree::ptree::value_type& pair, pt) {
				if (pair.first == "body") { // if author... get json array and process the authors as well. || pair.first == "title"
											//start = boost::chrono::high_resolution_clock::now();
					std::string input = pair.second.get_value<std::string>();
					std::transform(input.begin(), input.end(), input.begin(), ::tolower);
					//finish = boost::chrono::high_resolution_clock::now();
					//stringTime += (boost::chrono::duration_cast<boost::chrono::nanoseconds>(finish - start).count() / 1000000.0);


					Tokenizer tkzr(input);
					std::string token;
					//token.reserve(100);
					int posIndex = 0;
					bool hyphen = false;
					while (tkzr.nextToken(token, hyphen)) {// while not end of file.
														   // Get stem the token or retrieve the value from a cache
														   //start = boost::chrono::high_resolution_clock::now();
						if (!hyphen) {
							std::string stemmedToken = (cache.find(token) != cache.end())
								? cache[token] : stemmer.stem(token);
							idx->addTerm(stemmedToken, i, posIndex); // stemmedToken
						}
						else {
							std::string total = "";
							for (auto s : split(token)) {
								idx->addTerm(stemmer.stem(s), i, posIndex);
								total += s;
							}
							idx->addTerm(stemmer.stem(total), i, posIndex);
						}

						posIndex++;
					}
				}
			}
			//system("pause");
		}

		totalFinish = boost::chrono::high_resolution_clock::now();
		elapsedTime = (boost::chrono::duration_cast<boost::chrono::nanoseconds>(totalFinish - totalStart).count() / 1000000.0);


		//std::cout << "Total elapsed time for string time: " << stringTime << " ms." << std::endl;
		//std::cout << "Total elapsed time for File Read Time: " << fileReadTime << " ms." << std::endl;
		//std::cout << "Total elapsed time to fill Json Tree: " << treeTime << " ms." << std::endl;
		//std::cout << "Total elapsed time for Porter Stemmer: " << stemTime << " ms." << std::endl;
		std::cout << "Total elapsed time for Populate Index: " << elapsedTime << " ms." << std::endl;
	}

	/*
	* This method goes to a path and walks through the directory searching for all files that end
	* with the .json extension. The method will put all full file paths into a vector that is passed
	* by reference.
	*/
	static void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) {
		boost::filesystem::directory_iterator end_itr;
		std::unordered_set<std::string> fileSet;

		for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
			if (is_regular_file(itr->path())) {
				std::string s = itr->path().string();
				if (boost::algorithm::ends_with(s, ".json")) {
					std::replace(s.begin(), s.end(), '\\', '/');
					//std::cout << s << '\n';
					fileSet.insert(s);
				}
			}
		}

		mPathList.resize(fileSet.size());
		int i = 0;
		for (auto s : fileSet)
			mPathList[i++] = s;
	}

	static std::vector<std::string> split(std::string token) {
		std::vector<std::string> vect;
		std::stringstream ss(token);

		char c;
		std::string str = "";
		while (ss >> c)
		{
			if (c != '-') // if NOT hyphen
				str += c;
			else {
				vect.push_back(str);
				str = "";
			}
		}
		vect.push_back(str);

		return vect;
	}
};

#endif
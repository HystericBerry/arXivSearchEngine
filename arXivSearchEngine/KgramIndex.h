#ifndef KGRAM_INDEX_H
#define KGRAM_INDEX_H

#include <unordered_map>
#include <string>
#include <list>

/*
 * Takes in an integer n, and that is the number kgram's number of char grams.
 * Main needs to send in an int n and that is what the kgram separates to.
 *
 * Index for Kgrams to map from kgram to list of terms; has add and get (public)
 * functionality for the index; essentially Index object, and a few independent methods
 * that allow you to use kgram logic.
 */
class KgramIndex {

	int _kgramSize;
	
	/** The hashmap that represents the entire inverted index.*/
	std::unordered_map<std::string, std::list<std::string*>> _mIndex;
	// Where do I store this.

	/**This method is used to check if a kgram exists. Instead of catching an out of 
	 * bounds error everytime, we can check to avoid the expensive try/ catch overhead.*/
	bool hasKgram(const std::string &kgram) const;

	/**Returns the postings list for the requested kgram.
	 * i.e. getPostings('$st')
	 * will return the list of all the string terms that apply that Kgram.*/
	void addKgram(const std::string &kgram, const std::string &term);

public:
	
	/**Constructor where the kSize is the k in kgram.
	 * Designed to support k = 1 - 3; however may support more than 3.*/
	KgramIndex(int kSize);

	std::list<std::string> getTerms(const std::string &kgram) const 

	void addTerm(const std::string &term); //add kgram to string into index

	void vocab() const;

	/** This (public) method is independent of the Index.
	 * It is intended to be used for query terms, so the user can get the kgrams of the term.*/
	std::list<std::string> getGrams(const std::string &term)

	unsigned int getGramCount() const;

};

#endif

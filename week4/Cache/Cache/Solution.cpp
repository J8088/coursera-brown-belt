#include "Common.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>
#include <optional>

using namespace std;

class LruCache : public ICache {
public:
	LruCache(
		shared_ptr<IBooksUnpacker> books_unpacker,
		const Settings& s
	)
		: settings(s)
		, unpacker(move(books_unpacker))
	{
	}

	BookPtr GetBook(const string& book_name) override {
		lock_guard<mutex> guard(mtx);

		auto it = cache.find(book_name);
		int new_rank = ++max_rank;
		if (it != cache.end()) {
			it->second.rank = new_rank;
			return it->second.book;
		}

		auto book = unpacker->UnpackBook(book_name);
		int book_size = book->GetContent().size();
		while ((sum_memory > 0) && (sum_memory + book_size > settings.max_memory)) {
			RemoveLruItem();
		}
		if (sum_memory + book_size > settings.max_memory) {
			return move(book);
		}
		cache[book_name] = CacheItem{ new_rank, move(book) };
		sum_memory += book_size;
		return cache[book_name].book;
	}

private:
	struct CacheItem {
		int rank;
		shared_ptr<IBook> book;
	};

	using CacheItems = unordered_map<string, CacheItem>;

private:
	void RemoveLruItem() {
		auto it = min_element(
			cache.begin(), cache.end(),
			[](const CacheItems::value_type& lhs, const CacheItems::value_type& rhs) {
				return lhs.second.rank < rhs.second.rank;
			}
		);

		sum_memory -= it->second.book->GetContent().size();
		cache.erase(it);
	}

private:
	Settings settings;
	shared_ptr<IBooksUnpacker> unpacker;

	CacheItems cache;
	int sum_memory = 0;
	int max_rank = 0;
	mutex mtx;
};


unique_ptr<ICache> MakeCache(
	shared_ptr<IBooksUnpacker> books_unpacker,
	const ICache::Settings& settings
) {
	return make_unique<LruCache>(move(books_unpacker), settings);
}

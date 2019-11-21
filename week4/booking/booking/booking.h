#pragma once

#include <utility>

namespace RAII
{

template <typename Provider>
class Booking {
public:
	using BookingId = typename Provider::BookingId;

	Booking(Provider* p, int id)
		: provider(p)
	{
	}

	Booking(const Booking&) = delete;
	Booking(Booking&& other)
		: provider(other.provider)
	{
		other.provider = nullptr;
	}

	Booking& operator=(const Booking&) = delete;
	Booking& operator=(Booking&& other) {
		std::swap(provider, other.provider);
		std::swap(booking_id, other.booking_id);
		return *this;
	}

	~Booking() {
		if (provider) {
			provider->CancelOrComplete(*this);
		}
	}
private:
	Provider* provider;
	BookingId booking_id;
};

}

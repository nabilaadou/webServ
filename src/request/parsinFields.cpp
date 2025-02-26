#include "httpSession.hpp"

int httpSession::Request::parseFields(const bstring& buffer, size_t pos) {
	size_t size = buffer.size();
	while (pos < size) {
		switch (s.sstat)
		{
		case e_sstat::emptyline:
			/* code */
			break;
		
		default:
			break;
		}
	}
	return -1;
}
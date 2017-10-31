/*
* Copyright (C) 2015-2017, Kevin Brubeck Unhammer <unhammer@fsfe.org>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef fe64e9a18486d375_SUGGEST_H
#define fe64e9a18486d375_SUGGEST_H

#include <locale>
#include <codecvt>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <exception>

// divvun-gramcheck:
#include "util.hpp"
#include "json.hpp"
#include "checkertypes.hpp"
// xml:
#ifdef HAVE_LIBPUGIXML
#include <pugixml.hpp>
#endif
// hfst:
#include <hfst/HfstInputStream.h>
#include <hfst/HfstTransducer.h>
// variants:
#include "mapbox/variant.hpp"

namespace divvun {

using mapbox::util::variant;

using UStringVector = std::vector<std::u16string>;

using msgmap = std::unordered_map<lang, std::pair<ToggleIds, ToggleRes> >;	// msgs[lang] = make_pair(ToggleIds, ToggleRes)

using rel_id = size_t;
using relations = std::unordered_map<std::string, rel_id>;

inline std::string xml_raw_cdata(const pugi::xml_node& label) {
	std::ostringstream os;
	for(const auto& cc: label.children())
	{
		cc.print(os, "", pugi::format_raw);
	}
	return os.str();
}

inline variant<Nothing, std::pair<err_id, UStringVector>> pickErr(const std::map<std::u16string, UStringVector>& err,
								  const std::set<err_id>& ignores) {
	for(const auto& it : err) {
		if(ignores.find(it.first) == ignores.end()) {
			// TODO: currently we just pick the first unignored if there are several error types:
			return it;
		}
	}
	return Nothing();
}

enum RunState {
	flushing,
	eof
};

class Suggest {
	public:
		Suggest (const hfst::HfstTransducer* generator, divvun::msgmap msgs, bool verbose);
		Suggest (const std::string& gen_path, const std::string& msg_path, bool verbose);
		~Suggest() = default;

		void run(std::istream& is, std::ostream& os, bool json);

		std::vector<Err> run_errs(std::istream& is);
		void setIgnores(const std::set<err_id>& ignores);

		static const hfst::HfstTransducer *readTransducer(const std::string& file);
		static const hfst::HfstTransducer *readTransducer(const char* buff, const size_t size);
		static const hfst::HfstTransducer *readTransducer(std::istream& is);

		static const msgmap readMessages(const std::string& file);
		static const msgmap readMessages(const char* buff, const size_t size);

		const msgmap msgs;
	private:
		RunState run_json(std::istream& is, std::ostream& os);
		std::unique_ptr<const hfst::HfstTransducer> generator;
		std::set<err_id> ignores;
};

}

#endif

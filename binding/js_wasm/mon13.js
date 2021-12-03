class Service {
	constructor(lib, mem_offset=undefined) {
		this.instance = lib.instance;
		if(mem_offset) {
			this.mem_offset = mem_offset;
		}
		if(!mem_offset) {
			//Avoid colliding with memory in use.
			const old = this.instance.exports.memory.grow(1);
			this.mem_offset = old * 64 * 1024;
		}

		this.encoder = new TextEncoder();
		this.decoder = new TextDecoder();

		const ex = lib.instance.exports;
		this.gregorian = ex.mon13_gregorian;
		this.gregorian_year_zero = ex.mon13_gregorian_year_zero;
		this.tranquility = ex.mon13_tranquility;
		this.tranquility_year_zero = ex.mon13_tranquility_year_zero;
		this.holocene = ex.mon13_holocene;
		this.cotsworth = ex.mon13_cotsworth;
		this.julian = ex.mon13_julian;
		this.positivist = ex.mon13_positivist;
		this.symmetry454 = ex.mon13_symmetry454;
		this.symmetry010 = ex.mon13_symmetry010;
		this.ancient_egyptian = ex.mon13_ancient_egyptian;
		this.french_revolutionary_romme = ex.mon13_french_revolutionary_romme;
		this.french_revolutionary_romme_sub1 = ex.mon13_french_revolutionary_romme_sub1;


		this.names_en_US_gregorian = ex.mon13_names_en_US_gregorian;
		this.names_en_US_tranquility = ex.mon13_names_en_US_tranquility;
		this.names_en_US_holocene = ex.mon13_names_en_US_holocene;
		this.names_en_US_cotsworth = ex.mon13_names_en_US_cotsworth;
		this.names_en_US_julian = ex.mon13_names_en_US_julian;
		this.names_en_US_positivist = ex.mon13_names_en_US_positivist;
		this.names_en_US_symmetry454 = ex.mon13_names_en_US_symmetry454;
		this.names_en_US_symmetry010 = ex.mon13_names_en_US_symmetry010;
		this.names_en_US_ancient_egyptian = ex.mon13_names_en_US_ancient_egyptian;
		this.names_en_GB_french_revolutionary = ex.mon13_names_en_GB_french_revolutionary;
		this.names_en_GB_french_revolutionary_joke = ex.mon13_names_en_GB_french_revolutionary_joke;
		this.names_fr_FR_gregorian = ex.mon13_names_fr_FR_gregorian;
		this.names_fr_FR_julian = ex.mon13_names_fr_FR_julian;
		this.names_fr_FR_positivist = ex.mon13_names_fr_FR_positivist;
		this.names_fr_FR_french_revolutionary = ex.mon13_names_fr_FR_french_revolutionary;

		this.Weekday7 = {
			NO_WEEKDAY: 0,
			MONDAY: 1,
			TUESDAY: 2,
			WEDNESDAY: 3,
			THURSDAY: 4,
			FRIDAY: 5,
			SATURDAY: 6,
			SUNDAY: 7
		}

		this.Weekday10 = {
			PRIMIDI: 1,
			DUODI: 2,
			TRIDI: 3,
			QUARTIDI: 4,
			QUINTIDI: 5,
			SEXTIDI: 6,
			SEPTIDI: 7,
			OCTIDI: 8,
			NONIDI: 9,
			DECADI: 10,
		}
	}

	_statusToString(status) {
		const start_i = this.instance.exports.mon13_errorMessage(status);
		var end_i = 256;
		const long_buf = new Uint8Array(this.instance.exports.memory.buffer, start_i, end_i);
		for(var i = 0; i < end_i; i++) {
			if(long_buf[i] == 0) {
				end_i = i;
				break;
			}
		}
		const status_buf = new Int8Array(this.instance.exports.memory.buffer, start_i, end_i);
		return this.decoder.decode(status_buf);
	}

	_tail(status) {
		if(status !== 0) {
			throw new Error(this._statusToString(status));
		}
		const buf = new Int32Array(this.instance.exports.memory.buffer, this.mem_offset, 1);
		return buf[0];
	}

	_tail_u8(status) {
		if(status !== 0) {
			throw new Error(this._statusToString(status));
		}
		const buf = new Uint8Array(this.instance.exports.memory.buffer, this.mem_offset, 1);
		return buf[0];

	}

	_tail_u16(status) {
		if(status !== 0) {
			throw new Error(this._statusToString(status));
		}
		const buf = new Uint16Array(this.instance.exports.memory.buffer, this.mem_offset, 1);
		return buf[0];
	}


	validYmd(cal, ymd) {
		const res = this.instance.exports.mon13_validYmd(
			cal, ymd.year, ymd.month, ymd.day, this.mem_offset
		);
		return !!res;
	}

	mjdFromYmd(cal, ymd) {
		const status = this.instance.exports.mon13_mjdFromYmd(
			cal, ymd.year, ymd.month, ymd.day, this.mem_offset
		);
		return this._tail(status);
	}

	mjdFromUnix(unix_time) {
		const big_unix_time = BigInt(unix_time);
		const status = this.instance.exports.mon13_mjdFromUnix(
			big_unix_time, this.mem_offset
		);
		return this._tail(status);
	}

	mjdFromRd(rd) {
		const status = this.instance.exports.mon13_mjdFromRd(
			rd, this.mem_offset
		);
		return this._tail(status);
	}

	mjdToYmd(mjd, cal) {
		const status = this.instance.exports.mon13_mjdToYmd(
			mjd, cal, this.mem_offset, this.mem_offset + 4, this.mem_offset + 5
		);
		const year = this._tail(status);
		const buf = new Uint8Array(this.instance.exports.memory.buffer, this.mem_offset, 6);
		const month = buf[4];
		const day = buf[5];
		return {
			year: year,
			month: month,
			day: day
		};
	}

	mjdToUnix(mjd) {
		const status = this.instance.exports.mon13_mjdToUnix(
			mjd, this.mem_offset
		);
		if(status !== 0) {
			throw new Error(this._statusToString(status));
		}
		const buf = new BigInt64Array(this.instance.exports.memory.buffer, this.mem_offset, 8);
		return buf[0];
	}

	mjdToRd(mjd) {
		const status = this.instance.exports.mon13_mjdToRd(
			mjd, this.mem_offset
		);
		return this._tail(status);
	}

	mjdToIsLeapYear(mjd, cal) {
		const status = this.instance.exports.mon13_mjdToIsLeapYear(
			mjd, cal, this.mem_offset
		);
		return !!this._tail_u8(status);
	}

	mjdToDayOfWeek(mjd, cal) {
		const status = this.instance.exports.mon13_mjdToDayOfWeek(
			mjd, cal, this.mem_offset
		);
		return this._tail_u8(status);
	}

	mjdToDayOfYear(mjd, cal) {
		const status = this.instance.exports.mon13_mjdToDayOfYear(
			mjd, cal, this.mem_offset
		);
		return this._tail_u16(status);
	}

	addMonths(mjd, cal, month_offset) {
		const status = this.instance.exports.mon13_addMonths(
			mjd, cal, month_offset, this.mem_offset
		);
		return this._tail(status);
	}

	addYears(mjd, cal, year_offset) {
		const status = this.instance.exports.mon13_addYears(
			mjd, cal, year_offset, this.mem_offset
		);
		return this._tail(status);
	}

	diffMonths(mjd0, mjd1, cal) {
		const status = this.instance.exports.mon13_diffMonths(
			mjd0, mjd1, cal, this.mem_offset
		);
		return this._tail(status);
	}

	diffYears(mjd0, mjd1, cal) {
		const status = this.instance.exports.mon13_diffYears(
			mjd0, mjd1, cal, this.mem_offset
		);
		return this._tail(status);
	}

	_copyToWasm(view, offset) {
		const res = new Uint8Array(
			this.instance.exports.memory.buffer, offset, view.byteLength + 1
		);
		for(var i = 0; i < view.byteLength; i++) {
			res[i] = view[i];
		}
		res[view.byteLength] = 0;
		return res;
	}

	_setStringList(nlist_buf, nlist_i, list, offset) {
		if(!list) {
			nlist_buf[nlist_i] = 0;
			return offset;
		}

		nlist_buf[nlist_i] = offset;

		const s_list = new Int32Array(
			this.instance.exports.memory.buffer, offset, list.length + 1
		);
		offset = s_list.byteOffset + s_list.byteLength;
		for(var i = 0; i < list.length; i++) {
			const enc = this.encoder.encode(list[i]);
			s_list[i] = this._copyToWasm(enc, offset).byteOffset;
			offset += enc.byteLength + 1;
		}
		s_list[list.length] = 0;
		while(offset % 4 !== 0) {
			offset++;
		}
		return offset;
	}

	format(mjd, cal, arg3, arg4) {
		var nlist = arg3;
		var fmt = arg4;
		if(!fmt) {
			nlist = null;
			fmt = arg3;
		}

		const encoder = new TextEncoder();
		var offset = this.mem_offset;
		if(nlist && nlist.constructor !== WebAssembly.Global) {
			const old_nlist = nlist;
			nlist = offset;
			const nlist_buf = new Int32Array(
				this.instance.exports.memory.buffer, offset, 6
			);
			offset += nlist_buf.byteLength;
			offset = this._setStringList(
				nlist_buf, 0, old_nlist.month_list, offset
			);
			offset = this._setStringList(
				nlist_buf, 1, old_nlist.weekday_list, offset
			);
			offset = this._setStringList(
				nlist_buf, 2, old_nlist.era_list, offset
			);
			offset = this._setStringList(
				nlist_buf, 3, old_nlist.intercalary_list, offset
			);
			offset = this._setStringList(
				nlist_buf, 4, old_nlist.alt_intercalary_list, offset
			);

			nlist_buf[5] = offset;
			const name_view = this.encoder.encode(old_nlist.calendar_name);
			const name_arr = this._copyToWasm(name_view, offset);
			offset += name_arr.byteLength + 1;
		}

		const fmt_view = this.encoder.encode(fmt);
		const fmt_arr = this._copyToWasm(fmt_view, offset);

		const buflen = this.instance.exports.mon13_format(
			mjd, cal, nlist, fmt_arr.byteOffset, null, 0
		)
		if(buflen < 0) {
			throw new Error(this._statusToString(buflen));
		}

		const res_offset = fmt_arr.byteOffset + fmt_arr.byteLength;
		const status = this.instance.exports.mon13_format(
			mjd, cal, nlist, fmt_arr.byteOffset, res_offset, buflen + 1
		)
		if(status < 0) {
			throw new Error(this._statusToString(status));
		}
		const res = new Uint8Array(
			this.instance.exports.memory.buffer, res_offset, buflen //exclude \0
		);
		return this.decoder.decode(res);
	}
}

module.exports = Service;
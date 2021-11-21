const base = @import("base.zig");

const msg_unknown = "Unknown error";
const msg_null_cal = "Null calendar";
const msg_null_name = "Null name list";
const msg_null_format = "Null format";
const msg_null_input = "Null input";
const msg_null_result = "Null result";
const msg_null_date = "Null date";
const msg_overflow = "Overflow occurred (internal)";
const msg_bad_cal = "Invalid calendar (internal)";
const msg_date_not_found = "Date not found (internal)";
const msg_doy_not_found = "Day of year not found (internal)";
const msg_invalid_utf8 = "Invalid UTF8";
const msg_beyond_end_state = "Beyond format end state (internal)";
const msg_invalid_sequence = "Invalid format sequence";
const msg_failed_to_insert_null = "Failed to insert null character (internal)";
const msg_invalid_date = "Invalid date";

pub fn errorMessage(e: base.Err) [*:0]const u8 {
    return switch (e) {
        base.Err.Unknown => msg_unknown,
        base.Err.NullCalendar => msg_null_cal,
        base.Err.NullNameList => msg_null_name,
        base.Err.NullFormat => msg_null_format,
        base.Err.NullInput => msg_null_input,
        base.Err.NullResult => msg_null_result,
        base.Err.NullDate => msg_null_date,
        base.Err.Overflow => msg_overflow,
        base.Err.BadCalendar => msg_bad_cal,
        base.Err.DateNotFound => msg_date_not_found,
        base.Err.DoyNotFound => msg_doy_not_found,
        base.Err.InvalidUtf8 => msg_invalid_utf8,
        base.Err.BeyondEndState => msg_beyond_end_state,
        base.Err.InvalidSequence => msg_invalid_sequence,
        base.Err.FailedToInsertNullCharacter => msg_failed_to_insert_null,
        base.Err.InvalidDate => msg_invalid_date,
    };
}

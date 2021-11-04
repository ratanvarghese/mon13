const base = @import("../base.zig");
const cal_gr = @import("gregorian.zig");

pub const julian = base.Cal{
    .intercalary_list = cal_gr.gregorian.intercalary_list,
    .common_lookup_list = cal_gr.gregorian.common_lookup_list,
    .leap_lookup_list = cal_gr.gregorian.leap_lookup_list,
    .leap_cycle = .{
        .year_count = cal_gr.gregorian.leap_cycle.year_count,
        .leap_year_count = cal_gr.gregorian.leap_cycle.leap_year_count,
        .offset_years = cal_gr.gregorian.leap_cycle.offset_years,
        .common_days = cal_gr.gregorian.leap_cycle.common_days,
        .leap_days = cal_gr.gregorian.leap_cycle.leap_days,
        .offset_days = cal_gr.gregorian.leap_cycle.offset_days,
        .LEAP_GREGORIAN_SKIP = false,
        .LEAP_SYMMETRY = false,
    },
    .epoch_mjd = -678577, //1 Jan, 1 AD
    .start_weekday = cal_gr.gregorian.start_weekday,
    .week_length = cal_gr.gregorian.week_length,
    .common_month_max = cal_gr.gregorian.common_month_max,
    .leap_month_max = cal_gr.gregorian.leap_month_max,
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = cal_gr.gregorian.CAL_PERENNIAL,
};

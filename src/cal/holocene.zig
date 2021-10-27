const base = @import("../base.zig");
const cal_gr = @import("gregorian.zig");

pub const holocene = base.Cal{
    .intercalary_list = cal_gr.gregorian.intercalary_list,
    .common_lookup_list = cal_gr.gregorian.common_lookup_list,
    .leap_lookup_list = cal_gr.gregorian.leap_lookup_list,
    .leap_cycle = cal_gr.gregorian.leap_cycle,
    .epoch_mjd = -4331000, //1 Jan, 1 HE
    .start_weekday = cal_gr.gregorian.start_weekday,
    .week_length = cal_gr.gregorian.week_length,
    .common_month_max = cal_gr.gregorian.common_month_max,
    .leap_month_max = cal_gr.gregorian.leap_month_max,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = cal_gr.gregorian.CAL_PERENNIAL,
};

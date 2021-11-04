const base = @import("../base.zig");
const cal_gr = @import("gregorian.zig");

pub const holocene = base.Cal{
    .intercalary_list = cal_gr.gregorian.intercalary_list,
    .common_lookup_list = cal_gr.gregorian.common_lookup_list,
    .leap_lookup_list = cal_gr.gregorian.leap_lookup_list,
    .leap_cycle = cal_gr.gregorian.leap_cycle,
    .week = cal_gr.gregorian.week,
    .epoch_mjd = -4331000, //1 Jan, 1 HE
    .common_month_max = cal_gr.gregorian.common_month_max,
    .leap_month_max = cal_gr.gregorian.leap_month_max,
    .year0 = true,
};

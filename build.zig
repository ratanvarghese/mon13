const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const c_options = &[_][]const u8{ "-std=c99", "-fPIC", "-Wall", "-Werror", "-I./include" };

    const lib_c = b.addSharedLibrary("mon13c", "src/format.zig", b.version(0, 1, 1));
    lib_c.addIncludeDir("include");
    lib_c.addCSourceFile("src/cal.c", c_options);
    lib_c.addCSourceFile("src/calc.c", c_options);

    lib_c.setBuildMode(mode);
    lib_c.install();

    const lib_z = b.addSharedLibrary("mon13z", "src/main.zig", b.version(0, 2, 1));

    lib_z.setBuildMode(mode);
    lib_z.install();

    // var main_tests = b.addTest("src/test.zig");
    // main_tests.addIncludeDir("include");
    // main_tests.setBuildMode(mode);

    // const test_step = b.step("test", "Run library tests");
    // test_step.dependOn(&main_tests.step);
}

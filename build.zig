const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const c_options = &[_][]const u8{ "-std=c99", "-fPIC", "-Wall", "-Werror", "-I./include" };

    const lib = b.addSharedLibrary("mon13", "src/format.zig", b.version(0, 1, 1));
    lib.addIncludeDir("include");
    lib.addCSourceFile("src/cal.c", c_options);
    lib.addCSourceFile("src/calc.c", c_options);

    lib.setBuildMode(mode);
    lib.install();

    var main_tests = b.addTest("src/test.zig");
    main_tests.addIncludeDir("include");
    main_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&main_tests.step);
}

const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const c_options = &[_][]const u8{ "-std=c99", "-fPIC", "-Wall", "-Werror", "-I./include" };

    const lib = b.addSharedLibrary("mon13", "src/main.zig", b.version(0, 1, 1));
    lib.addCSourceFile("src/cal.c", c_options);
    lib.addCSourceFile("src/calc.c", c_options);
    lib.addCSourceFile("src/format.c", c_options);

    lib.setBuildMode(mode);
    lib.install();

    var main_tests = b.addTest("src/main.zig");
    main_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&main_tests.step);
}

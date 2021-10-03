const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const sharedLib = b.addSharedLibrary("mon13", "src/mon13.zig", b.version(0, 4, 1));

    sharedLib.setBuildMode(mode);
    sharedLib.install();

    const staticLib = b.addStaticLibrary("mon13", "src/mon13.zig");

    staticLib.setBuildMode(mode);
    staticLib.install();

    const mon13Pkg = std.build.Pkg{
        .name = "mon13",
        .path = "src/mon13.zig",
    };

    var main_tests = b.addTest("test/unit.zig");
    main_tests.addPackage(mon13Pkg);
    main_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&main_tests.step);
}

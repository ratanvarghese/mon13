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

    const propertyTestExe = b.addExecutable("ptest", "test/ptest.c");
    propertyTestExe.linkLibC();
    propertyTestExe.linkSystemLibraryName("m");
    propertyTestExe.linkSystemLibraryName("theft");
    propertyTestExe.linkLibrary(sharedLib);
    propertyTestExe.addLibPath("theft/build");
    propertyTestExe.addIncludeDir("include");
    propertyTestExe.addIncludeDir("test");
    propertyTestExe.addIncludeDir("theft/inc");
    propertyTestExe.setBuildMode(mode);
    propertyTestExe.install();

    const propertyTestRun = propertyTestExe.run();

    const mon13Pkg = std.build.Pkg{
        .name = "mon13",
        .path = "src/mon13.zig",
    };

    var main_tests = b.addTest("test/unit.zig");
    main_tests.addPackage(mon13Pkg);
    main_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&main_tests.step);
    test_step.dependOn(&propertyTestRun.step);
}

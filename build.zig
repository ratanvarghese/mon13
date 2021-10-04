const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    //Library files
    const sharedLib = b.addSharedLibrary("mon13", "src/mon13.zig", b.version(0, 4, 1));
    sharedLib.setBuildMode(mode);
    sharedLib.install();

    const staticLib = b.addStaticLibrary("mon13", "src/mon13.zig");
    staticLib.setBuildMode(mode);
    staticLib.install();

    //Unit tests
    const mon13Pkg = std.build.Pkg{
        .name = "mon13",
        .path = "src/mon13.zig",
    };

    var unitTests = b.addTest("test/unit.zig");
    unitTests.addPackage(mon13Pkg);
    unitTests.setBuildMode(mode);

    const testStep = b.step("utest", "Run unit tests");
    testStep.dependOn(&unitTests.step);

    //Theft tests
    const submoduleInit = b.addSystemCommand(&[_][]const u8{ "git", "submodule", "init" });
    const submoduleUpdate = b.addSystemCommand(&[_][]const u8{ "git", "submodule", "update" });
    submoduleUpdate.step.dependOn(&submoduleInit.step);

    const makeTheft = b.addSystemCommand(&[_][]const u8{ "make", "-C", "theft" });
    submoduleUpdate.step.dependOn(&submoduleUpdate.step);

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
    propertyTestExe.step.dependOn(&makeTheft.step);

    const propertyTestRun = propertyTestExe.run();

    const propertyTestStep = b.step("ptest", "Run property-based tests (POSIX only)");
    propertyTestStep.dependOn(&propertyTestRun.step);
}

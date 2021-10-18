const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();
    const mon13Pkg = std.build.Pkg{
        .name = "mon13",
        .path = "src/mon13.zig",
    };
    const rawSeed = b.option(u64, "seed", "Seed for ptest");

    //Library files
    const sharedLib = b.addSharedLibrary("mon13", "binding/c/bindc.zig", b.version(0, 4, 1));
    sharedLib.addPackage(mon13Pkg);
    sharedLib.setBuildMode(mode);
    sharedLib.install();

    const staticLib = b.addStaticLibrary("mon13", "binding/c/bindc.zig");
    staticLib.addPackage(mon13Pkg);
    staticLib.setBuildMode(mode);
    staticLib.install();

    //Unit tests
    var unitTests = b.addTest("test/unit.zig");
    unitTests.addPackage(mon13Pkg);
    unitTests.setBuildMode(mode);

    const testStep = b.step("utest", "Run unit tests");
    testStep.dependOn(&unitTests.step);

    //Property-based tests (using theft library)
    const submoduleInit = b.addSystemCommand(&[_][]const u8{ "git", "submodule", "init" });
    const submoduleUpdate = b.addSystemCommand(&[_][]const u8{ "git", "submodule", "update" });
    submoduleUpdate.step.dependOn(&submoduleInit.step);

    const makeTheft = b.addSystemCommand(&[_][]const u8{ "make", "-C", "theft" });
    makeTheft.step.dependOn(&submoduleUpdate.step);

    const propertyTestExe = b.addExecutable("ptest", "test/ptest.c");
    propertyTestExe.linkLibC();
    propertyTestExe.linkSystemLibraryName("m");
    propertyTestExe.linkSystemLibraryName("theft");
    propertyTestExe.linkLibrary(sharedLib);
    propertyTestExe.addLibPath("theft/build");
    propertyTestExe.addIncludeDir("binding/c");
    propertyTestExe.addIncludeDir("test");
    propertyTestExe.addIncludeDir("theft/inc");
    propertyTestExe.setBuildMode(mode);
    propertyTestExe.step.dependOn(&makeTheft.step);

    const propertyTestRun = propertyTestExe.run();
    if (rawSeed) |seed| {
        var seed_buf: [100]u8 = undefined;
        var seed_arg = std.fmt.bufPrint(
            seed_buf[0..seed_buf.len],
            "{d}",
            .{seed},
        ) catch unreachable;
        propertyTestRun.addArg(seed_arg);
    }

    const propertyTestStep = b.step("ptest", "Run property-based tests (POSIX only)");
    propertyTestStep.dependOn(&propertyTestRun.step);
}

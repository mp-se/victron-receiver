Import("env")
import shutil

def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)

def after_build(source, target, env):
    print( "Executing custom step " )
    dir    = env.GetLaunchDir()
    name   = env.get( "PIOENV" )
    if name == "victron-s3-waveshare-tft" :
        target = dir + "/bin/firmware32s3w_tft.bin"
        source = dir + "/.pio/build/" + name + "/firmware.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )

        target = dir + "/bin/partitions32s3w.bin"
        source = dir + "/.pio/build/" + name + "/partitions.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )
    elif name == "victron-lolin-tft" :
        target = dir + "/bin/firmware32_tft.bin"
        source = dir + "/.pio/build/" + name + "/firmware.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )

        target = dir + "/bin/partitions32.bin"
        source = dir + "/.pio/build/" + name + "/partitions.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )
    elif name == "victron-s3-lolin-tft" :
        target = dir + "/bin/firmware32s3_tft.bin"
        source = dir + "/.pio/build/" + name + "/firmware.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )

        target = dir + "/bin/partitions32s3.bin"
        source = dir + "/.pio/build/" + name + "/partitions.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )
    elif name == "victron-c3-lolin" :
        target = dir + "/bin/firmware32c3.bin"
        source = dir + "/.pio/build/" + name + "/firmware.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )

        target = dir + "/bin/partitions32c3.bin"
        source = dir + "/.pio/build/" + name + "/partitions.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )
    elif name == "victron-s3-lolin" :
        target = dir + "/bin/firmware32s3.bin"
        source = dir + "/.pio/build/" + name + "/firmware.bin"
        print( "Copy file : " + source + " -> " + target )
        shutil.copyfile( source, target )

        # target = dir + "/bin/partitions32s3.bin"
        # source = dir + "/.pio/build/" + name + "/partitions.bin"
        # print( "Copy file : " + source + " -> " + target )
        # shutil.copyfile( source, target )

print( "Adding custom build step (copy firmware): ")
env.AddPostAction("buildprog", after_build)

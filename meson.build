project('patty_cake', 'c', 'cpp', default_options : ['cpp_std=c++20', 'buildtype=debug'])


build_sys = build_machine.system()
if build_sys == 'linux'
  add_global_arguments('-D_LINUX', language : 'cpp')
elif build_sys == 'windows'
  add_global_arguments('-D_WINDOWS', language : 'cpp')
  add_global_arguments('-DBOOST_SYSTEM_NO_DEPRECATED', language : 'cpp')
  add_global_arguments('-w', language : 'cpp')
  add_global_arguments('/bigobj', language : 'cpp')
  add_global_arguments('-D_WIN32_WINNT=0x0601', language : 'cpp')
endif

add_global_arguments('-D_THREAD_SAFE', language : 'cpp')


## inc_dirs
inc_dirs = [
  include_directories('./external/webrtc/src'),
  include_directories('./external/webrtc/src/out/datachannelonly/gen'),
  include_directories('./external/webrtc/src/third_party/abseil-cpp'),
]
if build_sys == 'windows'
  inc_dirs += [
    include_directories('C:/vcpkg/installed/x64-windows/include'),
  ]
endif


## dependencies
# deps
deps = [
  dependency('ixwebsocket'),
  dependency('zlib'),

  declare_dependency(link_args:['../external/webrtc/src/out/datachannelonly/obj/pc/libpeer_connection_thick.a']),
]


## cpp_args
cpp_args = [
  '-stdlib=libc++',
  '-lc++abi',
]

link_args = [
  '-stdlib=libc++',
  '-lc++abi',
  '-lpthread',
  '-ldl',
]

link_with = [
  #static_library('peer_connection',   ['external/webrtc/src/out/datachannelonly/obj/pc/libpeer_connection_thick.a']),
  #static_library('api',              ['libapi.a']),
  #static_library('p2p',              ['libp2p.a']),
  #static_library('rtc_base',         ['librtc_base.a']),
  #static_library('rtc_base_approved',['librtc_base_approved.a']),
  #static_library('usrsctp',          ['libusrsctp.a']),
  #static_library('absl_synchronization', ['libabsl_synchronization.a']),
  #static_library('absl_base',        ['libabsl_base.a'])
]


# case of build_sys
if build_sys == 'windows'
  link_args = [
    '-LC:/vcpkg/installed/x64-windows/debug/lib',
    '-lwinmm',
    '-lws2_32',
  ]
elif build_sys == 'linux'
  cpp_args += [
    '-g',
  ]
  link_args += [
  ]
endif




##### build ####

#########################################################################
# test client
srcs = run_command('python3', 'find_src.py', 'src').stdout().strip().split('\n')
srcs += run_command('python3', 'find_src.py', 'test/client').stdout().strip().split('\n')

executable('client', srcs, dependencies : deps, include_directories : inc_dirs, link_args : link_args, cpp_args : cpp_args)


#########################################################################
# test server
srcs = run_command('python3', 'find_src.py', 'src').stdout().strip().split('\n')
srcs += run_command('python3', 'find_src.py', 'test/server').stdout().strip().split('\n')

executable('server', srcs, dependencies : deps, include_directories : inc_dirs, link_args : link_args, cpp_args : cpp_args)



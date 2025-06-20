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
  dependency('datachannel'),
]


## cpp_args
cpp_args = [
]

link_args = [
  '-lpthread',
  '-ldl',
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
srcs += run_command('python3', 'find_src.py', 'test/common').stdout().strip().split('\n')

executable('client', srcs, dependencies : deps, include_directories : inc_dirs, link_args : link_args, cpp_args : cpp_args)


#########################################################################
# test server
srcs = run_command('python3', 'find_src.py', 'src').stdout().strip().split('\n')
srcs += run_command('python3', 'find_src.py', 'test/server').stdout().strip().split('\n')
srcs += run_command('python3', 'find_src.py', 'test/common').stdout().strip().split('\n')

executable('server', srcs, dependencies : deps, include_directories : inc_dirs, link_args : link_args, cpp_args : cpp_args)



project('net_stack', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c11'
  ])

# 添加 include 目录
inc = include_directories('include')

# 获取所有源文件
src_files = files(
  'src/main.c',
  # 如果有其他 .c 文件，继续添加
  # 'src/other.c',
)

# 也可以自动查找所有 .c 文件
src_list = run_command('find', 'src', '-name', '*.c', check: true).stdout().strip().split('\n')
src_files = files(src_list)

# 创建可执行文件
executable('net_stack',
  src_files,
  include_directories : inc)
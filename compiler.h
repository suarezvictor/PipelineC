// Compiler helper stuff
#pragma once

// Temp work around for C++/bool types, see github issue
// https://github.com/JulianKemmerer/PipelineC/issues/24
#ifdef __PIPELINEC__
#define bool uint1_t
#define true 1
#define false 0
#define and &
#define or |
#else
#ifndef __cplusplus
#define and &&
#define or ||
#define uint1_t bool
#endif
#endif

// Can't parse attributes
// https://github.com/eliben/pycparser/wiki/FAQ#what-do-i-do-about-__attribute__
#ifdef __PIPELINEC__
#define __attribute__(x)
#endif

#define PRAGMA_MESSAGE(x) _Pragma(#x)

#define CLK_MHZ(clock, mhz)\
PRAGMA_MESSAGE(CLK_MHZ clock mhz)

#define MAIN(main_func)\
PRAGMA_MESSAGE(MAIN main_func)

#define MAIN_MHZ(main_func, mhz)\
PRAGMA_MESSAGE(MAIN_MHZ main_func mhz)

#define MAIN_SYN_MHZ(main_func, mhz)\
PRAGMA_MESSAGE(MAIN_SYN_MHZ main_func mhz)

#define MAIN_MHZ_GROUP(main_func, mhz, group)\
PRAGMA_MESSAGE(MAIN_MHZ main_func mhz group)

// Work around for user top level IO:
// https://github.com/JulianKemmerer/PipelineC/issues/123
// https://github.com/JulianKemmerer/PipelineC/issues/130
#define DECL_INPUT(type_t, name) \
type_t name; \
PRAGMA_MESSAGE(MAIN name) \
PRAGMA_MESSAGE(FUNC_WIRES name) \
void name(type_t val_input) \
{ \
  name = val_input;\
}
#define DECL_OUTPUT(type_t, name) \
type_t name; \
PRAGMA_MESSAGE(MAIN name) \
PRAGMA_MESSAGE(FUNC_WIRES name) \
type_t name() \
{ \
  return name; \
}

// Split a main funciton instance into N globally available function calls
// Maybe code gen some day?
#define MAIN_SPLIT2( \
  main_split_out_t, \
  main_name, \
  part_0_name, \
  part_0_name_in_t, \
  part_0_name_out_t, \
  part_1_name, \
  part_1_name_in_t, \
  part_1_name_out_t \
) \
/* 'ports' of global wires, can't be single array*/ \
part_0_name_in_t main_name##_##part_0_name##_in; \
part_0_name_out_t main_name##_##part_0_name##_out; \
part_1_name_in_t main_name##_##part_1_name##_in; \
part_1_name_out_t main_name##_##part_1_name##_out; \
/* One helper func per port */ \
part_0_name_out_t part_0_name(part_0_name_in_t inputs){ \
  main_name##_##part_0_name##_in = inputs; \
  return main_name##_##part_0_name##_out; \
} \
part_1_name_out_t part_1_name(part_1_name_in_t inputs){ \
  main_name##_##part_1_name##_in = inputs; \
  return main_name##_##part_1_name##_out; \
} \
/* And the actual main instance of the user's main func*/ \
MAIN(main_name##_SPLIT2) \
void main_name##_SPLIT2() \
{ \
  main_split_out_t main_out = main_name( \
    main_name##_##part_0_name##_in, \
    main_name##_##part_1_name##_in \
  );\
  main_name##_##part_0_name##_out = main_out.part_0_name; \
  main_name##_##part_1_name##_out = main_out.part_1_name; \
}
#define MAIN_SPLIT3( \
  main_split_out_t, \
  main_name, \
  part_0_name, \
  part_0_name_in_t, \
  part_0_name_out_t, \
  part_1_name, \
  part_1_name_in_t, \
  part_1_name_out_t, \
  part_2_name, \
  part_2_name_in_t, \
  part_2_name_out_t \
) \
/* 'ports' of global wires, can't be single array*/ \
part_0_name_in_t main_name##_##part_0_name##_in; \
part_0_name_out_t main_name##_##part_0_name##_out; \
part_1_name_in_t main_name##_##part_1_name##_in; \
part_1_name_out_t main_name##_##part_1_name##_out; \
part_2_name_in_t main_name##_##part_2_name##_in; \
part_2_name_out_t main_name##_##part_2_name##_out; \
/* One helper func per port */ \
part_0_name_out_t part_0_name(part_0_name_in_t inputs){ \
  main_name##_##part_0_name##_in = inputs; \
  return main_name##_##part_0_name##_out; \
} \
part_1_name_out_t part_1_name(part_1_name_in_t inputs){ \
  main_name##_##part_1_name##_in = inputs; \
  return main_name##_##part_1_name##_out; \
} \
part_2_name_out_t part_2_name(part_2_name_in_t inputs){ \
  main_name##_##part_2_name##_in = inputs; \
  return main_name##_##part_2_name##_out; \
} \
/* And the actual main instance of the user's main func*/ \
MAIN(main_name##_SPLIT3) \
void main_name##_SPLIT3() \
{ \
  main_split_out_t main_out = main_name( \
    main_name##_##part_0_name##_in, \
    main_name##_##part_1_name##_in, \
    main_name##_##part_2_name##_in \
  );\
  main_name##_##part_0_name##_out = main_out.part_0_name; \
  main_name##_##part_1_name##_out = main_out.part_1_name; \
  main_name##_##part_2_name##_out = main_out.part_2_name; \
}

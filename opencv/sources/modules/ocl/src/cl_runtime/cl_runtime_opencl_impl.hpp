//
// AUTOGENERATED, DO NOT EDIT
//
// generated by parser_cl.py
enum OPENCL_FN_ID {
    OPENCL_FN_clGetPlatformIDs = 0,
    OPENCL_FN_clGetPlatformInfo,
    OPENCL_FN_clGetDeviceIDs,
    OPENCL_FN_clGetDeviceInfo,
    OPENCL_FN_clCreateSubDevices,
    OPENCL_FN_clRetainDevice,
    OPENCL_FN_clReleaseDevice,
    OPENCL_FN_clCreateContext,
    OPENCL_FN_clCreateContextFromType,
    OPENCL_FN_clRetainContext,
    OPENCL_FN_clReleaseContext,
    OPENCL_FN_clGetContextInfo,
    OPENCL_FN_clCreateCommandQueue,
    OPENCL_FN_clRetainCommandQueue,
    OPENCL_FN_clReleaseCommandQueue,
    OPENCL_FN_clGetCommandQueueInfo,
    OPENCL_FN_clCreateBuffer,
    OPENCL_FN_clCreateSubBuffer,
    OPENCL_FN_clCreateImage,
    OPENCL_FN_clRetainMemObject,
    OPENCL_FN_clReleaseMemObject,
    OPENCL_FN_clGetSupportedImageFormats,
    OPENCL_FN_clGetMemObjectInfo,
    OPENCL_FN_clGetImageInfo,
    OPENCL_FN_clSetMemObjectDestructorCallback,
    OPENCL_FN_clCreateSampler,
    OPENCL_FN_clRetainSampler,
    OPENCL_FN_clReleaseSampler,
    OPENCL_FN_clGetSamplerInfo,
    OPENCL_FN_clCreateProgramWithSource,
    OPENCL_FN_clCreateProgramWithBinary,
    OPENCL_FN_clCreateProgramWithBuiltInKernels,
    OPENCL_FN_clRetainProgram,
    OPENCL_FN_clReleaseProgram,
    OPENCL_FN_clBuildProgram,
    OPENCL_FN_clCompileProgram,
    OPENCL_FN_clLinkProgram,
    OPENCL_FN_clUnloadPlatformCompiler,
    OPENCL_FN_clGetProgramInfo,
    OPENCL_FN_clGetProgramBuildInfo,
    OPENCL_FN_clCreateKernel,
    OPENCL_FN_clCreateKernelsInProgram,
    OPENCL_FN_clRetainKernel,
    OPENCL_FN_clReleaseKernel,
    OPENCL_FN_clSetKernelArg,
    OPENCL_FN_clGetKernelInfo,
    OPENCL_FN_clGetKernelArgInfo,
    OPENCL_FN_clGetKernelWorkGroupInfo,
    OPENCL_FN_clWaitForEvents,
    OPENCL_FN_clGetEventInfo,
    OPENCL_FN_clCreateUserEvent,
    OPENCL_FN_clRetainEvent,
    OPENCL_FN_clReleaseEvent,
    OPENCL_FN_clSetUserEventStatus,
    OPENCL_FN_clSetEventCallback,
    OPENCL_FN_clGetEventProfilingInfo,
    OPENCL_FN_clFlush,
    OPENCL_FN_clFinish,
    OPENCL_FN_clEnqueueReadBuffer,
    OPENCL_FN_clEnqueueReadBufferRect,
    OPENCL_FN_clEnqueueWriteBuffer,
    OPENCL_FN_clEnqueueWriteBufferRect,
    OPENCL_FN_clEnqueueFillBuffer,
    OPENCL_FN_clEnqueueCopyBuffer,
    OPENCL_FN_clEnqueueCopyBufferRect,
    OPENCL_FN_clEnqueueReadImage,
    OPENCL_FN_clEnqueueWriteImage,
    OPENCL_FN_clEnqueueFillImage,
    OPENCL_FN_clEnqueueCopyImage,
    OPENCL_FN_clEnqueueCopyImageToBuffer,
    OPENCL_FN_clEnqueueCopyBufferToImage,
    OPENCL_FN_clEnqueueMapBuffer,
    OPENCL_FN_clEnqueueMapImage,
    OPENCL_FN_clEnqueueUnmapMemObject,
    OPENCL_FN_clEnqueueMigrateMemObjects,
    OPENCL_FN_clEnqueueNDRangeKernel,
    OPENCL_FN_clEnqueueTask,
    OPENCL_FN_clEnqueueNativeKernel,
    OPENCL_FN_clEnqueueMarkerWithWaitList,
    OPENCL_FN_clEnqueueBarrierWithWaitList,
    OPENCL_FN_clGetExtensionFunctionAddressForPlatform,
    OPENCL_FN_clCreateImage2D,
    OPENCL_FN_clCreateImage3D,
    OPENCL_FN_clEnqueueMarker,
    OPENCL_FN_clEnqueueWaitForEvents,
    OPENCL_FN_clEnqueueBarrier,
    OPENCL_FN_clUnloadCompiler,
    OPENCL_FN_clGetExtensionFunctionAddress,
};
// generated by parser_cl.py
const char* opencl_fn_names[] = {
    "clGetPlatformIDs",
    "clGetPlatformInfo",
    "clGetDeviceIDs",
    "clGetDeviceInfo",
    "clCreateSubDevices",
    "clRetainDevice",
    "clReleaseDevice",
    "clCreateContext",
    "clCreateContextFromType",
    "clRetainContext",
    "clReleaseContext",
    "clGetContextInfo",
    "clCreateCommandQueue",
    "clRetainCommandQueue",
    "clReleaseCommandQueue",
    "clGetCommandQueueInfo",
    "clCreateBuffer",
    "clCreateSubBuffer",
    "clCreateImage",
    "clRetainMemObject",
    "clReleaseMemObject",
    "clGetSupportedImageFormats",
    "clGetMemObjectInfo",
    "clGetImageInfo",
    "clSetMemObjectDestructorCallback",
    "clCreateSampler",
    "clRetainSampler",
    "clReleaseSampler",
    "clGetSamplerInfo",
    "clCreateProgramWithSource",
    "clCreateProgramWithBinary",
    "clCreateProgramWithBuiltInKernels",
    "clRetainProgram",
    "clReleaseProgram",
    "clBuildProgram",
    "clCompileProgram",
    "clLinkProgram",
    "clUnloadPlatformCompiler",
    "clGetProgramInfo",
    "clGetProgramBuildInfo",
    "clCreateKernel",
    "clCreateKernelsInProgram",
    "clRetainKernel",
    "clReleaseKernel",
    "clSetKernelArg",
    "clGetKernelInfo",
    "clGetKernelArgInfo",
    "clGetKernelWorkGroupInfo",
    "clWaitForEvents",
    "clGetEventInfo",
    "clCreateUserEvent",
    "clRetainEvent",
    "clReleaseEvent",
    "clSetUserEventStatus",
    "clSetEventCallback",
    "clGetEventProfilingInfo",
    "clFlush",
    "clFinish",
    "clEnqueueReadBuffer",
    "clEnqueueReadBufferRect",
    "clEnqueueWriteBuffer",
    "clEnqueueWriteBufferRect",
    "clEnqueueFillBuffer",
    "clEnqueueCopyBuffer",
    "clEnqueueCopyBufferRect",
    "clEnqueueReadImage",
    "clEnqueueWriteImage",
    "clEnqueueFillImage",
    "clEnqueueCopyImage",
    "clEnqueueCopyImageToBuffer",
    "clEnqueueCopyBufferToImage",
    "clEnqueueMapBuffer",
    "clEnqueueMapImage",
    "clEnqueueUnmapMemObject",
    "clEnqueueMigrateMemObjects",
    "clEnqueueNDRangeKernel",
    "clEnqueueTask",
    "clEnqueueNativeKernel",
    "clEnqueueMarkerWithWaitList",
    "clEnqueueBarrierWithWaitList",
    "clGetExtensionFunctionAddressForPlatform",
    "clCreateImage2D",
    "clCreateImage3D",
    "clEnqueueMarker",
    "clEnqueueWaitForEvents",
    "clEnqueueBarrier",
    "clUnloadCompiler",
    "clGetExtensionFunctionAddress",
};

namespace {
// generated by parser_cl.py
#define opencl_fn0(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(); } \

#define opencl_fn1(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1); } \

#define opencl_fn2(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2); } \

#define opencl_fn3(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3); } \

#define opencl_fn4(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4); } \

#define opencl_fn5(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5); } \

#define opencl_fn6(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6); } \

#define opencl_fn7(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7); } \

#define opencl_fn8(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8); } \

#define opencl_fn9(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9); } \

#define opencl_fn10(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); } \

#define opencl_fn11(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); } \

#define opencl_fn12(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); } \

#define opencl_fn13(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); } \

#define opencl_fn14(ID, _R, decl_args) \
    typedef _R (CL_API_CALL*ID##FN)decl_args; \
    static _R CL_API_CALL ID##_switch_fn decl_args \
    { return ((ID##FN)opencl_check_fn(ID))(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14); } \

}

// generated by parser_cl.py
opencl_fn3(OPENCL_FN_clGetPlatformIDs, cl_int, (cl_uint p1, cl_platform_id* p2, cl_uint* p3))
cl_int (CL_API_CALL*clGetPlatformIDs)(cl_uint, cl_platform_id*, cl_uint*) =
        OPENCL_FN_clGetPlatformIDs_switch_fn;
opencl_fn5(OPENCL_FN_clGetPlatformInfo, cl_int, (cl_platform_id p1, cl_platform_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetPlatformInfo)(cl_platform_id, cl_platform_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetPlatformInfo_switch_fn;
opencl_fn5(OPENCL_FN_clGetDeviceIDs, cl_int, (cl_platform_id p1, cl_device_type p2, cl_uint p3, cl_device_id* p4, cl_uint* p5))
cl_int (CL_API_CALL*clGetDeviceIDs)(cl_platform_id, cl_device_type, cl_uint, cl_device_id*, cl_uint*) =
        OPENCL_FN_clGetDeviceIDs_switch_fn;
opencl_fn5(OPENCL_FN_clGetDeviceInfo, cl_int, (cl_device_id p1, cl_device_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetDeviceInfo)(cl_device_id, cl_device_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetDeviceInfo_switch_fn;
opencl_fn5(OPENCL_FN_clCreateSubDevices, cl_int, (cl_device_id p1, const cl_device_partition_property* p2, cl_uint p3, cl_device_id* p4, cl_uint* p5))
cl_int (CL_API_CALL*clCreateSubDevices)(cl_device_id, const cl_device_partition_property*, cl_uint, cl_device_id*, cl_uint*) =
        OPENCL_FN_clCreateSubDevices_switch_fn;
opencl_fn1(OPENCL_FN_clRetainDevice, cl_int, (cl_device_id p1))
cl_int (CL_API_CALL*clRetainDevice)(cl_device_id) =
        OPENCL_FN_clRetainDevice_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseDevice, cl_int, (cl_device_id p1))
cl_int (CL_API_CALL*clReleaseDevice)(cl_device_id) =
        OPENCL_FN_clReleaseDevice_switch_fn;
opencl_fn6(OPENCL_FN_clCreateContext, cl_context, (const cl_context_properties* p1, cl_uint p2, const cl_device_id* p3, void (CL_CALLBACK*p4) (const char*, const void*, size_t, void*), void* p5, cl_int* p6))
cl_context (CL_API_CALL*clCreateContext)(const cl_context_properties*, cl_uint, const cl_device_id*, void (CL_CALLBACK*) (const char*, const void*, size_t, void*), void*, cl_int*) =
        OPENCL_FN_clCreateContext_switch_fn;
opencl_fn5(OPENCL_FN_clCreateContextFromType, cl_context, (const cl_context_properties* p1, cl_device_type p2, void (CL_CALLBACK*p3) (const char*, const void*, size_t, void*), void* p4, cl_int* p5))
cl_context (CL_API_CALL*clCreateContextFromType)(const cl_context_properties*, cl_device_type, void (CL_CALLBACK*) (const char*, const void*, size_t, void*), void*, cl_int*) =
        OPENCL_FN_clCreateContextFromType_switch_fn;
opencl_fn1(OPENCL_FN_clRetainContext, cl_int, (cl_context p1))
cl_int (CL_API_CALL*clRetainContext)(cl_context) =
        OPENCL_FN_clRetainContext_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseContext, cl_int, (cl_context p1))
cl_int (CL_API_CALL*clReleaseContext)(cl_context) =
        OPENCL_FN_clReleaseContext_switch_fn;
opencl_fn5(OPENCL_FN_clGetContextInfo, cl_int, (cl_context p1, cl_context_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetContextInfo)(cl_context, cl_context_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetContextInfo_switch_fn;
opencl_fn4(OPENCL_FN_clCreateCommandQueue, cl_command_queue, (cl_context p1, cl_device_id p2, cl_command_queue_properties p3, cl_int* p4))
cl_command_queue (CL_API_CALL*clCreateCommandQueue)(cl_context, cl_device_id, cl_command_queue_properties, cl_int*) =
        OPENCL_FN_clCreateCommandQueue_switch_fn;
opencl_fn1(OPENCL_FN_clRetainCommandQueue, cl_int, (cl_command_queue p1))
cl_int (CL_API_CALL*clRetainCommandQueue)(cl_command_queue) =
        OPENCL_FN_clRetainCommandQueue_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseCommandQueue, cl_int, (cl_command_queue p1))
cl_int (CL_API_CALL*clReleaseCommandQueue)(cl_command_queue) =
        OPENCL_FN_clReleaseCommandQueue_switch_fn;
opencl_fn5(OPENCL_FN_clGetCommandQueueInfo, cl_int, (cl_command_queue p1, cl_command_queue_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetCommandQueueInfo)(cl_command_queue, cl_command_queue_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetCommandQueueInfo_switch_fn;
opencl_fn5(OPENCL_FN_clCreateBuffer, cl_mem, (cl_context p1, cl_mem_flags p2, size_t p3, void* p4, cl_int* p5))
cl_mem (CL_API_CALL*clCreateBuffer)(cl_context, cl_mem_flags, size_t, void*, cl_int*) =
        OPENCL_FN_clCreateBuffer_switch_fn;
opencl_fn5(OPENCL_FN_clCreateSubBuffer, cl_mem, (cl_mem p1, cl_mem_flags p2, cl_buffer_create_type p3, const void* p4, cl_int* p5))
cl_mem (CL_API_CALL*clCreateSubBuffer)(cl_mem, cl_mem_flags, cl_buffer_create_type, const void*, cl_int*) =
        OPENCL_FN_clCreateSubBuffer_switch_fn;
opencl_fn6(OPENCL_FN_clCreateImage, cl_mem, (cl_context p1, cl_mem_flags p2, const cl_image_format* p3, const cl_image_desc* p4, void* p5, cl_int* p6))
cl_mem (CL_API_CALL*clCreateImage)(cl_context, cl_mem_flags, const cl_image_format*, const cl_image_desc*, void*, cl_int*) =
        OPENCL_FN_clCreateImage_switch_fn;
opencl_fn1(OPENCL_FN_clRetainMemObject, cl_int, (cl_mem p1))
cl_int (CL_API_CALL*clRetainMemObject)(cl_mem) =
        OPENCL_FN_clRetainMemObject_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseMemObject, cl_int, (cl_mem p1))
cl_int (CL_API_CALL*clReleaseMemObject)(cl_mem) =
        OPENCL_FN_clReleaseMemObject_switch_fn;
opencl_fn6(OPENCL_FN_clGetSupportedImageFormats, cl_int, (cl_context p1, cl_mem_flags p2, cl_mem_object_type p3, cl_uint p4, cl_image_format* p5, cl_uint* p6))
cl_int (CL_API_CALL*clGetSupportedImageFormats)(cl_context, cl_mem_flags, cl_mem_object_type, cl_uint, cl_image_format*, cl_uint*) =
        OPENCL_FN_clGetSupportedImageFormats_switch_fn;
opencl_fn5(OPENCL_FN_clGetMemObjectInfo, cl_int, (cl_mem p1, cl_mem_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetMemObjectInfo)(cl_mem, cl_mem_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetMemObjectInfo_switch_fn;
opencl_fn5(OPENCL_FN_clGetImageInfo, cl_int, (cl_mem p1, cl_image_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetImageInfo)(cl_mem, cl_image_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetImageInfo_switch_fn;
opencl_fn3(OPENCL_FN_clSetMemObjectDestructorCallback, cl_int, (cl_mem p1, void (CL_CALLBACK*p2) (cl_mem, void*), void* p3))
cl_int (CL_API_CALL*clSetMemObjectDestructorCallback)(cl_mem, void (CL_CALLBACK*) (cl_mem, void*), void*) =
        OPENCL_FN_clSetMemObjectDestructorCallback_switch_fn;
opencl_fn5(OPENCL_FN_clCreateSampler, cl_sampler, (cl_context p1, cl_bool p2, cl_addressing_mode p3, cl_filter_mode p4, cl_int* p5))
cl_sampler (CL_API_CALL*clCreateSampler)(cl_context, cl_bool, cl_addressing_mode, cl_filter_mode, cl_int*) =
        OPENCL_FN_clCreateSampler_switch_fn;
opencl_fn1(OPENCL_FN_clRetainSampler, cl_int, (cl_sampler p1))
cl_int (CL_API_CALL*clRetainSampler)(cl_sampler) =
        OPENCL_FN_clRetainSampler_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseSampler, cl_int, (cl_sampler p1))
cl_int (CL_API_CALL*clReleaseSampler)(cl_sampler) =
        OPENCL_FN_clReleaseSampler_switch_fn;
opencl_fn5(OPENCL_FN_clGetSamplerInfo, cl_int, (cl_sampler p1, cl_sampler_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetSamplerInfo)(cl_sampler, cl_sampler_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetSamplerInfo_switch_fn;
opencl_fn5(OPENCL_FN_clCreateProgramWithSource, cl_program, (cl_context p1, cl_uint p2, const char** p3, const size_t* p4, cl_int* p5))
cl_program (CL_API_CALL*clCreateProgramWithSource)(cl_context, cl_uint, const char**, const size_t*, cl_int*) =
        OPENCL_FN_clCreateProgramWithSource_switch_fn;
opencl_fn7(OPENCL_FN_clCreateProgramWithBinary, cl_program, (cl_context p1, cl_uint p2, const cl_device_id* p3, const size_t* p4, const unsigned char** p5, cl_int* p6, cl_int* p7))
cl_program (CL_API_CALL*clCreateProgramWithBinary)(cl_context, cl_uint, const cl_device_id*, const size_t*, const unsigned char**, cl_int*, cl_int*) =
        OPENCL_FN_clCreateProgramWithBinary_switch_fn;
opencl_fn5(OPENCL_FN_clCreateProgramWithBuiltInKernels, cl_program, (cl_context p1, cl_uint p2, const cl_device_id* p3, const char* p4, cl_int* p5))
cl_program (CL_API_CALL*clCreateProgramWithBuiltInKernels)(cl_context, cl_uint, const cl_device_id*, const char*, cl_int*) =
        OPENCL_FN_clCreateProgramWithBuiltInKernels_switch_fn;
opencl_fn1(OPENCL_FN_clRetainProgram, cl_int, (cl_program p1))
cl_int (CL_API_CALL*clRetainProgram)(cl_program) =
        OPENCL_FN_clRetainProgram_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseProgram, cl_int, (cl_program p1))
cl_int (CL_API_CALL*clReleaseProgram)(cl_program) =
        OPENCL_FN_clReleaseProgram_switch_fn;
opencl_fn6(OPENCL_FN_clBuildProgram, cl_int, (cl_program p1, cl_uint p2, const cl_device_id* p3, const char* p4, void (CL_CALLBACK*p5) (cl_program, void*), void* p6))
cl_int (CL_API_CALL*clBuildProgram)(cl_program, cl_uint, const cl_device_id*, const char*, void (CL_CALLBACK*) (cl_program, void*), void*) =
        OPENCL_FN_clBuildProgram_switch_fn;
opencl_fn9(OPENCL_FN_clCompileProgram, cl_int, (cl_program p1, cl_uint p2, const cl_device_id* p3, const char* p4, cl_uint p5, const cl_program* p6, const char** p7, void (CL_CALLBACK*p8) (cl_program, void*), void* p9))
cl_int (CL_API_CALL*clCompileProgram)(cl_program, cl_uint, const cl_device_id*, const char*, cl_uint, const cl_program*, const char**, void (CL_CALLBACK*) (cl_program, void*), void*) =
        OPENCL_FN_clCompileProgram_switch_fn;
opencl_fn9(OPENCL_FN_clLinkProgram, cl_program, (cl_context p1, cl_uint p2, const cl_device_id* p3, const char* p4, cl_uint p5, const cl_program* p6, void (CL_CALLBACK*p7) (cl_program, void*), void* p8, cl_int* p9))
cl_program (CL_API_CALL*clLinkProgram)(cl_context, cl_uint, const cl_device_id*, const char*, cl_uint, const cl_program*, void (CL_CALLBACK*) (cl_program, void*), void*, cl_int*) =
        OPENCL_FN_clLinkProgram_switch_fn;
opencl_fn1(OPENCL_FN_clUnloadPlatformCompiler, cl_int, (cl_platform_id p1))
cl_int (CL_API_CALL*clUnloadPlatformCompiler)(cl_platform_id) =
        OPENCL_FN_clUnloadPlatformCompiler_switch_fn;
opencl_fn5(OPENCL_FN_clGetProgramInfo, cl_int, (cl_program p1, cl_program_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetProgramInfo)(cl_program, cl_program_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetProgramInfo_switch_fn;
opencl_fn6(OPENCL_FN_clGetProgramBuildInfo, cl_int, (cl_program p1, cl_device_id p2, cl_program_build_info p3, size_t p4, void* p5, size_t* p6))
cl_int (CL_API_CALL*clGetProgramBuildInfo)(cl_program, cl_device_id, cl_program_build_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetProgramBuildInfo_switch_fn;
opencl_fn3(OPENCL_FN_clCreateKernel, cl_kernel, (cl_program p1, const char* p2, cl_int* p3))
cl_kernel (CL_API_CALL*clCreateKernel)(cl_program, const char*, cl_int*) =
        OPENCL_FN_clCreateKernel_switch_fn;
opencl_fn4(OPENCL_FN_clCreateKernelsInProgram, cl_int, (cl_program p1, cl_uint p2, cl_kernel* p3, cl_uint* p4))
cl_int (CL_API_CALL*clCreateKernelsInProgram)(cl_program, cl_uint, cl_kernel*, cl_uint*) =
        OPENCL_FN_clCreateKernelsInProgram_switch_fn;
opencl_fn1(OPENCL_FN_clRetainKernel, cl_int, (cl_kernel p1))
cl_int (CL_API_CALL*clRetainKernel)(cl_kernel) =
        OPENCL_FN_clRetainKernel_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseKernel, cl_int, (cl_kernel p1))
cl_int (CL_API_CALL*clReleaseKernel)(cl_kernel) =
        OPENCL_FN_clReleaseKernel_switch_fn;
opencl_fn4(OPENCL_FN_clSetKernelArg, cl_int, (cl_kernel p1, cl_uint p2, size_t p3, const void* p4))
cl_int (CL_API_CALL*clSetKernelArg)(cl_kernel, cl_uint, size_t, const void*) =
        OPENCL_FN_clSetKernelArg_switch_fn;
opencl_fn5(OPENCL_FN_clGetKernelInfo, cl_int, (cl_kernel p1, cl_kernel_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetKernelInfo)(cl_kernel, cl_kernel_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetKernelInfo_switch_fn;
opencl_fn6(OPENCL_FN_clGetKernelArgInfo, cl_int, (cl_kernel p1, cl_uint p2, cl_kernel_arg_info p3, size_t p4, void* p5, size_t* p6))
cl_int (CL_API_CALL*clGetKernelArgInfo)(cl_kernel, cl_uint, cl_kernel_arg_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetKernelArgInfo_switch_fn;
opencl_fn6(OPENCL_FN_clGetKernelWorkGroupInfo, cl_int, (cl_kernel p1, cl_device_id p2, cl_kernel_work_group_info p3, size_t p4, void* p5, size_t* p6))
cl_int (CL_API_CALL*clGetKernelWorkGroupInfo)(cl_kernel, cl_device_id, cl_kernel_work_group_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetKernelWorkGroupInfo_switch_fn;
opencl_fn2(OPENCL_FN_clWaitForEvents, cl_int, (cl_uint p1, const cl_event* p2))
cl_int (CL_API_CALL*clWaitForEvents)(cl_uint, const cl_event*) =
        OPENCL_FN_clWaitForEvents_switch_fn;
opencl_fn5(OPENCL_FN_clGetEventInfo, cl_int, (cl_event p1, cl_event_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetEventInfo)(cl_event, cl_event_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetEventInfo_switch_fn;
opencl_fn2(OPENCL_FN_clCreateUserEvent, cl_event, (cl_context p1, cl_int* p2))
cl_event (CL_API_CALL*clCreateUserEvent)(cl_context, cl_int*) =
        OPENCL_FN_clCreateUserEvent_switch_fn;
opencl_fn1(OPENCL_FN_clRetainEvent, cl_int, (cl_event p1))
cl_int (CL_API_CALL*clRetainEvent)(cl_event) =
        OPENCL_FN_clRetainEvent_switch_fn;
opencl_fn1(OPENCL_FN_clReleaseEvent, cl_int, (cl_event p1))
cl_int (CL_API_CALL*clReleaseEvent)(cl_event) =
        OPENCL_FN_clReleaseEvent_switch_fn;
opencl_fn2(OPENCL_FN_clSetUserEventStatus, cl_int, (cl_event p1, cl_int p2))
cl_int (CL_API_CALL*clSetUserEventStatus)(cl_event, cl_int) =
        OPENCL_FN_clSetUserEventStatus_switch_fn;
opencl_fn4(OPENCL_FN_clSetEventCallback, cl_int, (cl_event p1, cl_int p2, void (CL_CALLBACK*p3) (cl_event, cl_int, void*), void* p4))
cl_int (CL_API_CALL*clSetEventCallback)(cl_event, cl_int, void (CL_CALLBACK*) (cl_event, cl_int, void*), void*) =
        OPENCL_FN_clSetEventCallback_switch_fn;
opencl_fn5(OPENCL_FN_clGetEventProfilingInfo, cl_int, (cl_event p1, cl_profiling_info p2, size_t p3, void* p4, size_t* p5))
cl_int (CL_API_CALL*clGetEventProfilingInfo)(cl_event, cl_profiling_info, size_t, void*, size_t*) =
        OPENCL_FN_clGetEventProfilingInfo_switch_fn;
opencl_fn1(OPENCL_FN_clFlush, cl_int, (cl_command_queue p1))
cl_int (CL_API_CALL*clFlush)(cl_command_queue) =
        OPENCL_FN_clFlush_switch_fn;
opencl_fn1(OPENCL_FN_clFinish, cl_int, (cl_command_queue p1))
cl_int (CL_API_CALL*clFinish)(cl_command_queue) =
        OPENCL_FN_clFinish_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueReadBuffer, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, size_t p4, size_t p5, void* p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueReadBuffer)(cl_command_queue, cl_mem, cl_bool, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueReadBuffer_switch_fn;
opencl_fn14(OPENCL_FN_clEnqueueReadBufferRect, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, const size_t* p4, const size_t* p5, const size_t* p6, size_t p7, size_t p8, size_t p9, size_t p10, void* p11, cl_uint p12, const cl_event* p13, cl_event* p14))
cl_int (CL_API_CALL*clEnqueueReadBufferRect)(cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueReadBufferRect_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueWriteBuffer, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, size_t p4, size_t p5, const void* p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueWriteBuffer)(cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueWriteBuffer_switch_fn;
opencl_fn14(OPENCL_FN_clEnqueueWriteBufferRect, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, const size_t* p4, const size_t* p5, const size_t* p6, size_t p7, size_t p8, size_t p9, size_t p10, const void* p11, cl_uint p12, const cl_event* p13, cl_event* p14))
cl_int (CL_API_CALL*clEnqueueWriteBufferRect)(cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueWriteBufferRect_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueFillBuffer, cl_int, (cl_command_queue p1, cl_mem p2, const void* p3, size_t p4, size_t p5, size_t p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueFillBuffer)(cl_command_queue, cl_mem, const void*, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueFillBuffer_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueCopyBuffer, cl_int, (cl_command_queue p1, cl_mem p2, cl_mem p3, size_t p4, size_t p5, size_t p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueCopyBuffer)(cl_command_queue, cl_mem, cl_mem, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueCopyBuffer_switch_fn;
opencl_fn13(OPENCL_FN_clEnqueueCopyBufferRect, cl_int, (cl_command_queue p1, cl_mem p2, cl_mem p3, const size_t* p4, const size_t* p5, const size_t* p6, size_t p7, size_t p8, size_t p9, size_t p10, cl_uint p11, const cl_event* p12, cl_event* p13))
cl_int (CL_API_CALL*clEnqueueCopyBufferRect)(cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueCopyBufferRect_switch_fn;
opencl_fn11(OPENCL_FN_clEnqueueReadImage, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, const size_t* p4, const size_t* p5, size_t p6, size_t p7, void* p8, cl_uint p9, const cl_event* p10, cl_event* p11))
cl_int (CL_API_CALL*clEnqueueReadImage)(cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueReadImage_switch_fn;
opencl_fn11(OPENCL_FN_clEnqueueWriteImage, cl_int, (cl_command_queue p1, cl_mem p2, cl_bool p3, const size_t* p4, const size_t* p5, size_t p6, size_t p7, const void* p8, cl_uint p9, const cl_event* p10, cl_event* p11))
cl_int (CL_API_CALL*clEnqueueWriteImage)(cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueWriteImage_switch_fn;
opencl_fn8(OPENCL_FN_clEnqueueFillImage, cl_int, (cl_command_queue p1, cl_mem p2, const void* p3, const size_t* p4, const size_t* p5, cl_uint p6, const cl_event* p7, cl_event* p8))
cl_int (CL_API_CALL*clEnqueueFillImage)(cl_command_queue, cl_mem, const void*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueFillImage_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueCopyImage, cl_int, (cl_command_queue p1, cl_mem p2, cl_mem p3, const size_t* p4, const size_t* p5, const size_t* p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueCopyImage)(cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueCopyImage_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueCopyImageToBuffer, cl_int, (cl_command_queue p1, cl_mem p2, cl_mem p3, const size_t* p4, const size_t* p5, size_t p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueCopyImageToBuffer)(cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, size_t, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueCopyImageToBuffer_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueCopyBufferToImage, cl_int, (cl_command_queue p1, cl_mem p2, cl_mem p3, size_t p4, const size_t* p5, const size_t* p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueCopyBufferToImage)(cl_command_queue, cl_mem, cl_mem, size_t, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueCopyBufferToImage_switch_fn;
opencl_fn10(OPENCL_FN_clEnqueueMapBuffer, void*, (cl_command_queue p1, cl_mem p2, cl_bool p3, cl_map_flags p4, size_t p5, size_t p6, cl_uint p7, const cl_event* p8, cl_event* p9, cl_int* p10))
void* (CL_API_CALL*clEnqueueMapBuffer)(cl_command_queue, cl_mem, cl_bool, cl_map_flags, size_t, size_t, cl_uint, const cl_event*, cl_event*, cl_int*) =
        OPENCL_FN_clEnqueueMapBuffer_switch_fn;
opencl_fn12(OPENCL_FN_clEnqueueMapImage, void*, (cl_command_queue p1, cl_mem p2, cl_bool p3, cl_map_flags p4, const size_t* p5, const size_t* p6, size_t* p7, size_t* p8, cl_uint p9, const cl_event* p10, cl_event* p11, cl_int* p12))
void* (CL_API_CALL*clEnqueueMapImage)(cl_command_queue, cl_mem, cl_bool, cl_map_flags, const size_t*, const size_t*, size_t*, size_t*, cl_uint, const cl_event*, cl_event*, cl_int*) =
        OPENCL_FN_clEnqueueMapImage_switch_fn;
opencl_fn6(OPENCL_FN_clEnqueueUnmapMemObject, cl_int, (cl_command_queue p1, cl_mem p2, void* p3, cl_uint p4, const cl_event* p5, cl_event* p6))
cl_int (CL_API_CALL*clEnqueueUnmapMemObject)(cl_command_queue, cl_mem, void*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueUnmapMemObject_switch_fn;
opencl_fn7(OPENCL_FN_clEnqueueMigrateMemObjects, cl_int, (cl_command_queue p1, cl_uint p2, const cl_mem* p3, cl_mem_migration_flags p4, cl_uint p5, const cl_event* p6, cl_event* p7))
cl_int (CL_API_CALL*clEnqueueMigrateMemObjects)(cl_command_queue, cl_uint, const cl_mem*, cl_mem_migration_flags, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueMigrateMemObjects_switch_fn;
opencl_fn9(OPENCL_FN_clEnqueueNDRangeKernel, cl_int, (cl_command_queue p1, cl_kernel p2, cl_uint p3, const size_t* p4, const size_t* p5, const size_t* p6, cl_uint p7, const cl_event* p8, cl_event* p9))
cl_int (CL_API_CALL*clEnqueueNDRangeKernel)(cl_command_queue, cl_kernel, cl_uint, const size_t*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueNDRangeKernel_switch_fn;
opencl_fn5(OPENCL_FN_clEnqueueTask, cl_int, (cl_command_queue p1, cl_kernel p2, cl_uint p3, const cl_event* p4, cl_event* p5))
cl_int (CL_API_CALL*clEnqueueTask)(cl_command_queue, cl_kernel, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueTask_switch_fn;
opencl_fn10(OPENCL_FN_clEnqueueNativeKernel, cl_int, (cl_command_queue p1, void (CL_CALLBACK*p2) (void*), void* p3, size_t p4, cl_uint p5, const cl_mem* p6, const void** p7, cl_uint p8, const cl_event* p9, cl_event* p10))
cl_int (CL_API_CALL*clEnqueueNativeKernel)(cl_command_queue, void (CL_CALLBACK*) (void*), void*, size_t, cl_uint, const cl_mem*, const void**, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueNativeKernel_switch_fn;
opencl_fn4(OPENCL_FN_clEnqueueMarkerWithWaitList, cl_int, (cl_command_queue p1, cl_uint p2, const cl_event* p3, cl_event* p4))
cl_int (CL_API_CALL*clEnqueueMarkerWithWaitList)(cl_command_queue, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueMarkerWithWaitList_switch_fn;
opencl_fn4(OPENCL_FN_clEnqueueBarrierWithWaitList, cl_int, (cl_command_queue p1, cl_uint p2, const cl_event* p3, cl_event* p4))
cl_int (CL_API_CALL*clEnqueueBarrierWithWaitList)(cl_command_queue, cl_uint, const cl_event*, cl_event*) =
        OPENCL_FN_clEnqueueBarrierWithWaitList_switch_fn;
opencl_fn2(OPENCL_FN_clGetExtensionFunctionAddressForPlatform, void*, (cl_platform_id p1, const char* p2))
void* (CL_API_CALL*clGetExtensionFunctionAddressForPlatform)(cl_platform_id, const char*) =
        OPENCL_FN_clGetExtensionFunctionAddressForPlatform_switch_fn;
opencl_fn8(OPENCL_FN_clCreateImage2D, cl_mem, (cl_context p1, cl_mem_flags p2, const cl_image_format* p3, size_t p4, size_t p5, size_t p6, void* p7, cl_int* p8))
cl_mem (CL_API_CALL*clCreateImage2D)(cl_context, cl_mem_flags, const cl_image_format*, size_t, size_t, size_t, void*, cl_int*) =
        OPENCL_FN_clCreateImage2D_switch_fn;
opencl_fn10(OPENCL_FN_clCreateImage3D, cl_mem, (cl_context p1, cl_mem_flags p2, const cl_image_format* p3, size_t p4, size_t p5, size_t p6, size_t p7, size_t p8, void* p9, cl_int* p10))
cl_mem (CL_API_CALL*clCreateImage3D)(cl_context, cl_mem_flags, const cl_image_format*, size_t, size_t, size_t, size_t, size_t, void*, cl_int*) =
        OPENCL_FN_clCreateImage3D_switch_fn;
opencl_fn2(OPENCL_FN_clEnqueueMarker, cl_int, (cl_command_queue p1, cl_event* p2))
cl_int (CL_API_CALL*clEnqueueMarker)(cl_command_queue, cl_event*) =
        OPENCL_FN_clEnqueueMarker_switch_fn;
opencl_fn3(OPENCL_FN_clEnqueueWaitForEvents, cl_int, (cl_command_queue p1, cl_uint p2, const cl_event* p3))
cl_int (CL_API_CALL*clEnqueueWaitForEvents)(cl_command_queue, cl_uint, const cl_event*) =
        OPENCL_FN_clEnqueueWaitForEvents_switch_fn;
opencl_fn1(OPENCL_FN_clEnqueueBarrier, cl_int, (cl_command_queue p1))
cl_int (CL_API_CALL*clEnqueueBarrier)(cl_command_queue) =
        OPENCL_FN_clEnqueueBarrier_switch_fn;
opencl_fn0(OPENCL_FN_clUnloadCompiler, cl_int, ())
cl_int (CL_API_CALL*clUnloadCompiler)() =
        OPENCL_FN_clUnloadCompiler_switch_fn;
opencl_fn1(OPENCL_FN_clGetExtensionFunctionAddress, void*, (const char* p1))
void* (CL_API_CALL*clGetExtensionFunctionAddress)(const char*) =
        OPENCL_FN_clGetExtensionFunctionAddress_switch_fn;

// generated by parser_cl.py
void* opencl_fn_ptrs[] = {
    &clGetPlatformIDs,
    &clGetPlatformInfo,
    &clGetDeviceIDs,
    &clGetDeviceInfo,
    &clCreateSubDevices,
    &clRetainDevice,
    &clReleaseDevice,
    &clCreateContext,
    &clCreateContextFromType,
    &clRetainContext,
    &clReleaseContext,
    &clGetContextInfo,
    &clCreateCommandQueue,
    &clRetainCommandQueue,
    &clReleaseCommandQueue,
    &clGetCommandQueueInfo,
    &clCreateBuffer,
    &clCreateSubBuffer,
    &clCreateImage,
    &clRetainMemObject,
    &clReleaseMemObject,
    &clGetSupportedImageFormats,
    &clGetMemObjectInfo,
    &clGetImageInfo,
    &clSetMemObjectDestructorCallback,
    &clCreateSampler,
    &clRetainSampler,
    &clReleaseSampler,
    &clGetSamplerInfo,
    &clCreateProgramWithSource,
    &clCreateProgramWithBinary,
    &clCreateProgramWithBuiltInKernels,
    &clRetainProgram,
    &clReleaseProgram,
    &clBuildProgram,
    &clCompileProgram,
    &clLinkProgram,
    &clUnloadPlatformCompiler,
    &clGetProgramInfo,
    &clGetProgramBuildInfo,
    &clCreateKernel,
    &clCreateKernelsInProgram,
    &clRetainKernel,
    &clReleaseKernel,
    &clSetKernelArg,
    &clGetKernelInfo,
    &clGetKernelArgInfo,
    &clGetKernelWorkGroupInfo,
    &clWaitForEvents,
    &clGetEventInfo,
    &clCreateUserEvent,
    &clRetainEvent,
    &clReleaseEvent,
    &clSetUserEventStatus,
    &clSetEventCallback,
    &clGetEventProfilingInfo,
    &clFlush,
    &clFinish,
    &clEnqueueReadBuffer,
    &clEnqueueReadBufferRect,
    &clEnqueueWriteBuffer,
    &clEnqueueWriteBufferRect,
    &clEnqueueFillBuffer,
    &clEnqueueCopyBuffer,
    &clEnqueueCopyBufferRect,
    &clEnqueueReadImage,
    &clEnqueueWriteImage,
    &clEnqueueFillImage,
    &clEnqueueCopyImage,
    &clEnqueueCopyImageToBuffer,
    &clEnqueueCopyBufferToImage,
    &clEnqueueMapBuffer,
    &clEnqueueMapImage,
    &clEnqueueUnmapMemObject,
    &clEnqueueMigrateMemObjects,
    &clEnqueueNDRangeKernel,
    &clEnqueueTask,
    &clEnqueueNativeKernel,
    &clEnqueueMarkerWithWaitList,
    &clEnqueueBarrierWithWaitList,
    &clGetExtensionFunctionAddressForPlatform,
    &clCreateImage2D,
    &clCreateImage3D,
    &clEnqueueMarker,
    &clEnqueueWaitForEvents,
    &clEnqueueBarrier,
    &clUnloadCompiler,
    &clGetExtensionFunctionAddress,
};

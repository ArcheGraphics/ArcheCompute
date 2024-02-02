"""

        ArcheCompute python module
        -----------------------

        .. currentmodule:: arche_compute

        .. autosummary::
           :toctree: _generate

           add
           subtract
    
"""
from __future__ import annotations
import pybind11_stubgen.typing_ext
import typing
__all__ = ['Array', 'DEVELOPER_TOOLS', 'DebugCaptureOption', 'DebugCaptureOptionOutput', 'DebugCaptureScope', 'Dtype', 'GPU_TRACE_DOCUMENT', 'Kernel', 'KernelBuilder', 'float16', 'float2', 'float2x2', 'float3', 'float32', 'float3x3', 'float4', 'float4x4', 'int16', 'int32', 'int64', 'int8', 'quat', 'spatial_matrix', 'spatial_vector', 'synchronize', 'transform', 'uint16', 'uint32', 'uint64', 'uint8']
class Array:
    """
    An N-dimensional array object.
    """
    @staticmethod
    def __init__(*args, **kwargs):
        """
        
                    __init__(self: array, val: Union[scalar, list, tuple, numpy.ndarray, array], dtype: Optional[Dtype] = None)
                  
        """
    def __len__(self) -> int:
        ...
    def item(self) -> typing.Any:
        """
                    Access the value of a scalar array.
        
                    Returns:
                        Standard Python scalar.
        """
    def tolist(self) -> typing.Any:
        """
                    Convert the array to a Python :class:`list`.
        
                    Returns:
                        list: The Python list.
        
                        If the array is a scalar then a standard Python scalar is returned.
        
                        If the array has more than one dimension then the result is a nested
                        list of lists.
        
                        The value type of the list corresponding to the last dimension is either
                        ``int`` or ``float`` depending on the ``dtype`` of the array.
        """
    @property
    def dtype(self) -> Dtype:
        """
                    The array's :class:`Dtype`.
        """
    @property
    def itemsize(self) -> int:
        """
        The size of the array's datatype in bytes.
        """
    @property
    def nbytes(self) -> int:
        """
        The number of bytes in the array.
        """
    @property
    def ndim(self) -> int:
        """
        The array's dimension.
        """
    @property
    def shape(self) -> tuple:
        """
                  The shape of the array as a Python list.
        
                  Returns:
                    tuple(int): A tuple containing the sizes of each dimension.
        """
    @property
    def size(self) -> int:
        """
        Number of elements in the array.
        """
class DebugCaptureOption:
    file_name: str
    output: DebugCaptureOptionOutput
    def __init__(self) -> None:
        ...
class DebugCaptureOptionOutput:
    """
    Members:
    
      DEVELOPER_TOOLS
    
      GPU_TRACE_DOCUMENT
    """
    DEVELOPER_TOOLS: typing.ClassVar[DebugCaptureOptionOutput]  # value = <DebugCaptureOptionOutput.DEVELOPER_TOOLS: 0>
    GPU_TRACE_DOCUMENT: typing.ClassVar[DebugCaptureOptionOutput]  # value = <DebugCaptureOptionOutput.GPU_TRACE_DOCUMENT: 1>
    __members__: typing.ClassVar[dict[str, DebugCaptureOptionOutput]]  # value = {'DEVELOPER_TOOLS': <DebugCaptureOptionOutput.DEVELOPER_TOOLS: 0>, 'GPU_TRACE_DOCUMENT': <DebugCaptureOptionOutput.GPU_TRACE_DOCUMENT: 1>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __ne__(self, other: typing.Any) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    def __setstate__(self, state: int) -> None:
        ...
    def __str__(self) -> str:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def value(self) -> int:
        ...
class DebugCaptureScope:
    def mark_begin(self) -> None:
        ...
    def mark_end(self) -> None:
        ...
    def start_debug_capture(self) -> None:
        ...
    def stop_debug_capture(self) -> None:
        ...
class Dtype:
    """
    
          An object to hold the type of a :class:`array`.
    
          See the :ref:`list of types <data_types>` for more details
          on available data types.
          
    """
    def __eq__(self, arg0: Dtype) -> bool:
        ...
    def __hash__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    @property
    def size(self) -> int:
        """
        Size of the type in bytes.
        """
class Kernel:
    @staticmethod
    def builder() -> KernelBuilder:
        ...
    def launch(self, thread_groups_per_grid: typing.Annotated[list[int], pybind11_stubgen.typing_ext.FixedSize(3)], threads_per_thread_group: typing.Annotated[list[int], pybind11_stubgen.typing_ext.FixedSize(3)], args: list[None | Array | list[int]], stream: int = 0) -> None:
        ...
class KernelBuilder:
    def build(self) -> Kernel:
        ...
    def entry(self, arg0: str) -> KernelBuilder:
        ...
    def hash_name(self, arg0: str) -> KernelBuilder:
        ...
    def lib_name(self, arg0: str) -> KernelBuilder:
        ...
    def source(self, arg0: str) -> KernelBuilder:
        ...
def synchronize(wait: bool, stream: int = 0) -> None:
    ...
DEVELOPER_TOOLS: DebugCaptureOptionOutput  # value = <DebugCaptureOptionOutput.DEVELOPER_TOOLS: 0>
GPU_TRACE_DOCUMENT: DebugCaptureOptionOutput  # value = <DebugCaptureOptionOutput.GPU_TRACE_DOCUMENT: 1>
float16: Dtype  # value = arc.float16
float2: Dtype  # value = arc.float2
float2x2: Dtype  # value = arc.float2x2
float3: Dtype  # value = arc.float3
float32: Dtype  # value = arc.float32
float3x3: Dtype  # value = arc.float3x3
float4: Dtype  # value = arc.float4
float4x4: Dtype  # value = arc.float4x4
int16: Dtype  # value = arc.int16
int32: Dtype  # value = arc.int32
int64: Dtype  # value = arc.int64
int8: Dtype  # value = arc.int8
quat: Dtype  # value = arc.quat
spatial_matrix: Dtype  # value = arc.spatial_matrix
spatial_vector: Dtype  # value = arc.spatial_vector
transform: Dtype  # value = arc.transform
uint16: Dtype  # value = arc.uint16
uint32: Dtype  # value = arc.uint32
uint64: Dtype  # value = arc.uint64
uint8: Dtype  # value = arc.uint8

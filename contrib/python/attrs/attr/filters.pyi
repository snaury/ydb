from typing import Any, Union 
 
from . import Attribute, _FilterType

 
def include(*what: Union[type, Attribute[Any]]) -> _FilterType[Any]: ...
def exclude(*what: Union[type, Attribute[Any]]) -> _FilterType[Any]: ...

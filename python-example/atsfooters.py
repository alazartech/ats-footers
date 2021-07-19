import os
from collections import namedtuple
from ctypes import (
    c_bool,
    c_char_p,
    c_int16,
    c_size_t,
    c_uint32,
    c_uint64,
    c_uint,
    c_void_p,
    create_string_buffer,
    CDLL,
    POINTER,
    Structure,
)


class EnumerationType(type(c_uint)):
    def __new__(metacls, name, bases, dict):
        if not "_members_" in dict:
            _members_ = {}
            for key, value in dict.items():
                if not key.startswith("_"):
                    _members_[key] = value
            dict["_members_"] = _members_
        cls = type(c_uint).__new__(metacls, name, bases, dict)
        for key, value in cls._members_.items():
            globals()[key] = value
        return cls

    def __contains__(self, value):
        return value in self._members_.values()

    def __repr__(self):
        return "<Enumeration %s>" % self.__name__


class Enumeration(c_uint):
    __metaclass__ = EnumerationType
    _members_ = {}

    def _update_name(self):
        for k, v in self._members_.items():
            if v == self.value:
                self.name = k
                break
        else:
            raise ValueError("No enumeration member with value %r" % self.value)

    def __init__(self, value):
        c_uint.__init__(self, value)
        self._update_name()

    @classmethod
    def from_param(cls, param):
        if isinstance(param, Enumeration):
            if param.__class__ != cls:
                raise ValueError("Cannot mix enumeration members")
            else:
                return param
        else:
            return cls(param)

    def __repr__(self):
        self._update_name()
        return "<member %s=%d of %r>" % (self.name, self.value, self.__class__)



def _rccheck(result, func, arguments):
    if (result != 0):
        raise Exception("Error calling function %s with arguments %s : %s" %
                        (func.__name__, str(arguments), str(result)))

class PrintableStructure(Structure):
    def __repr__(self):
        values = ", ".join("{}={}".format(name, value)
                          for name, value in self._asdict().items())
        return "<{}: {}>".format(self.__class__.__name__, values)

    def _asdict(self):
        return {field[0]: getattr(self, field[0])
                for field in self._fields_}

class FooterType(Enumeration):
    type_0 = 0
    type_1 = 1

class FooterType0(PrintableStructure):
    _fields_ = [
        ("trigger_timestamp", c_uint64 ),
        ("record_number", c_uint32 ),
        ("frame_count", c_uint32 ),
        ("aux_in_state", c_bool ),
        ]

class FooterType1(PrintableStructure):
    _fields_ = [
        ("trigger_timestamp", c_uint64 ),
        ("record_number", c_uint32 ),
        ("frame_count", c_uint32 ),
        ("aux_in_state", c_bool ),
        ("analog_value", c_int16 ),
        ]

class BoardType(Enumeration):
    ats850 = 1
    ats310 = 2
    ats330 = 3
    ats855 = 4
    ats315 = 5
    ats335 = 6
    ats460 = 7
    ats860 = 8
    ats660 = 9
    ats665 = 10
    ats9462 = 11
    ats9434 = 12
    ats9870 = 13
    ats9350 = 14
    ats9325 = 15
    ats9440 = 16
    ats9410 = 17
    ats9351 = 18
    ats9310 = 19
    ats9461 = 20
    ats9850 = 21
    ats9625 = 22
    atg6500 = 23
    ats9626 = 24
    ats9360 = 25
    axi9870 = 26
    ats9370 = 27
    atu7825 = 28
    ats9373 = 29
    ats9416 = 30
    ats9637 = 31
    ats9120 = 32
    ats9371 = 33
    ats9130 = 34
    ats9352 = 35
    ats9453 = 36
    ats9146 = 37
    ats9000 = 38
    atst371 = 39
    ats9437 = 40
    ats9618 = 41
    ats9358 = 42
    forest = 43
    ats9353 = 44
    ats9872 = 45
    ats9470 = 46
    ats9628 = 47

class DataDomain(Enumeration):
    time = 0x1000
    frequency = 0x2000

class DataLayout(Enumeration):
    buffer_interleaved = 0x100000
    record_interleaved = 0x200000
    sample_interleaved = 0x300000

class FooterConfiguration(PrintableStructure):
    _fields_ = [
        ("board_type", BoardType),
        ("data_domain", DataDomain),
        ("active_channel_count", c_size_t),
        ("data_layout", DataLayout),
        ("bytes_per_record_per_channel", c_size_t),
        ("records_per_buffer_per_channel", c_size_t),
        ("fifo", c_bool),
    ]

class ATSFooters():
    def __init__(self):
        os.add_dll_directory(os.path.dirname(__file__))
        self.lib = CDLL("atsfooters.dll")

        self.lib.c_ats_parse_footers_type_0.restype = c_uint32
        self.lib.c_ats_parse_footers_type_0.argtypes = [c_void_p, c_size_t, FooterConfiguration, POINTER(FooterType0), c_size_t, c_char_p, c_size_t]
        self.lib.c_ats_parse_footers_type_0.errcheck = _rccheck

        self.lib.c_ats_parse_footers_type_1.restype = c_uint32
        self.lib.c_ats_parse_footers_type_1.argtypes = [c_void_p, c_size_t, FooterConfiguration, POINTER(FooterType1), c_size_t, c_char_p, c_size_t]
        self.lib.c_ats_parse_footers_type_1.errcheck = _rccheck

    def parse_type_0(self, np_data, footer_configuration, footer_count):
        footers = (FooterType0 * footer_count)()
        errstrsize = 256
        errstr = create_string_buffer(errstrsize)
        self.lib.c_ats_parse_footers_type_0(
            np_data.ctypes.data, np_data.size * np_data.itemsize, footer_configuration, footers, footer_count, errstr, errstrsize)
        return footers

    def parse_type_1(self, np_data, footer_configuration, footer_count):
        footers = (FooterType1 * footer_count)()
        errstrsize = 256
        errstr = create_string_buffer(errstrsize)
        self.lib.c_ats_parse_footers_type_1(
            np_data.ctypes.data, np_data.size * np_data.itemsize, footer_configuration, footers, footer_count, errstr, errstrsize)
        return footers

using System;
using System.Runtime.InteropServices;
using System.Security;


namespace Footers
{
    public unsafe partial class FootersAPI
    {
        /// <summary>Identifies the ATS footer types that various products generate</summary>
        public enum AtsFooterType
        {
            Type0 = 0,
            Type1 = 1
        }

        /// <summary>AlazarTech products</summary>
        public enum AtsBoardType
        {
            Ats850 = 1,
            Ats310 = 2,
            Ats330 = 3,
            Ats855 = 4,
            Ats315 = 5,
            Ats335 = 6,
            Ats460 = 7,
            Ats860 = 8,
            Ats660 = 9,
            Ats665 = 10,
            Ats9462 = 11,
            Ats9434 = 12,
            Ats9870 = 13,
            Ats9350 = 14,
            Ats9325 = 15,
            Ats9440 = 16,
            Ats9410 = 17,
            Ats9351 = 18,
            Ats9310 = 19,
            Ats9461 = 20,
            Ats9850 = 21,
            Ats9625 = 22,
            Atg6500 = 23,
            Ats9626 = 24,
            Ats9360 = 25,
            Axi9870 = 26,
            Ats9370 = 27,
            Atu7825 = 28,
            Ats9373 = 29,
            Ats9416 = 30,
            Ats9637 = 31,
            Ats9120 = 32,
            Ats9371 = 33,
            Ats9130 = 34,
            Ats9352 = 35,
            Ats9453 = 36,
            Ats9146 = 37,
            Ats9000 = 38,
            Atst371 = 39,
            Ats9437 = 40,
            Ats9618 = 41,
            Ats9358 = 42,
            Forest = 43,
            Ats9353 = 44,
            Ats9872 = 45,
            Ats9470 = 46,
            Ats9628 = 47
        }

        [Flags]
        public enum AtsDataDomain
        {
            Time = 4096,
            Frequency = 8192
        }

        public enum AtsDataLayout
        {
            BufferInterleaved = 1048576,
            RecordInterleaved = 2097152,
            SampleInterleaved = 3145728
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct AtsFooterType0
        {
            public UInt64 TriggerTimestamp;
            public UInt32 RecordNumber;
            public UInt32 FrameCount;
            public byte AuxInState;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct AtsFooterType1
        {
            public UInt64 TriggerTimestamp;
            public UInt32 RecordNumber;
            public UInt32 FrameCount;
            public byte AuxInState;
            public UInt16 AnalogValue;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct AtsFooterConfiguration {
            public AtsBoardType BoardType;
            public AtsDataDomain DataDomain;
            public UInt64 ActiveChannelCount;
            public AtsDataLayout DataLayout;
            public UInt64 BytesPerRecordPerChannel;
            public UInt64 RecordsPerBufferPerChannel;
            public byte Fifo;
        }

        [DllImport("atsfooters", EntryPoint="get_ats_footer_type")]
        public static extern unsafe AtsFooterType GetAtsFooterType(AtsBoardType board_type);

        [DllImport("atsfooters", EntryPoint="c_ats_parse_footers_type_0")]
        public static extern unsafe int CAtsParseFootersType0(
            byte[] buffer,
            ulong data_size_bytes,
            AtsFooterConfiguration configuration,
            [Out] AtsFooterType0[] footers,
            ulong footer_count,
            [Out] byte[] error_message,
            ulong error_message_max_size);

        [DllImport("atsfooters", EntryPoint="c_ats_parse_footers_type_1")]
        public static extern unsafe int CAtsParseFootersType1(
            byte[] buffer,
            ulong data_size_bytes,
            AtsFooterConfiguration configuration,
            [Out] AtsFooterType1[] footers,
            ulong footer_count,
            [Out] byte[] error_message,
            ulong error_message_max_size);
    }
}

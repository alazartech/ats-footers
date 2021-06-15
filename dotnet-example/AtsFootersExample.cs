using System;
using System.Text;
using System.IO;

using Footers;

namespace dotnet_example
{
    class Program
    {
        static void Main(string[] args)
        {
            byte[] data = File.ReadAllBytes("data-ats9130-2ch-fifo.bin");

            FootersAPI.AtsFooterConfiguration configuration = new FootersAPI.AtsFooterConfiguration();
            configuration.ActiveChannelCount = 2;
            configuration.BoardType = FootersAPI.AtsBoardType.Ats9130;
            configuration.BytesPerRecordPerChannel = 2048 * 2;
            configuration.DataDomain = FootersAPI.AtsDataDomain.Time;
            configuration.DataLayout = FootersAPI.AtsDataLayout.SampleInterleaved;
            configuration.RecordsPerBufferPerChannel = 2;
            FootersAPI.AtsFooterType0[] footers = new FootersAPI.AtsFooterType0[4];
            for (int i = 0; i < footers.Length; ++i)
            {
                footers[i] = new FootersAPI.AtsFooterType0();
            }
            ulong errorMessageSize = 256;
            byte[] errorMessage = new byte[errorMessageSize];
            int result = FootersAPI.CAtsParseFootersType0(data, Convert.ToUInt64(data.Length),
                                             configuration, footers, Convert.ToUInt64(footers.Length), errorMessage, errorMessageSize);
            if (result != 0) {
                string error = Encoding.Default.GetString(errorMessage);
                System.Console.Write("Parsing footers failed: {0}", error);
                return;
            }
            foreach (FootersAPI.AtsFooterType0 footer in footers) {
                System.Console.Write("{0}, ", footer.RecordNumber);
            }
        }
    }
}

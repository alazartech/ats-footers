import numpy as np

from atsfooters import (
    ATSFooters,
    FooterConfiguration,
    BoardType,
    DataDomain,
    DataLayout,
)


def main():
    data = np.fromfile("data-ats9350-fft-1ch-2112.bin", np.uint8)
    config = FooterConfiguration(BoardType.ats9350, DataDomain.frequency, 1,
                                 DataLayout.sample_interleaved, 2112, 2, False)
    lib = ATSFooters()
    footers = lib.parse_type_0(data, config, 4)
    for f in footers:
        print(f)

if __name__ == "__main__":
    main()

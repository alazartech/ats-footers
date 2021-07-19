import numpy as np

from atsfooters import (
    ATSFooters,
    FooterConfiguration,
    BoardType,
    DataDomain,
    DataLayout,
)


def main():
    data = np.fromfile("data.bin", np.uint8)
    config = FooterConfiguration(BoardType.ats9352, DataDomain.frequency, 2,
                                 DataLayout.record_interleaved, 4128, 2, False)
    lib = ATSFooters()
    footers = lib.parse_type_1(data, config, 4)
    for f in footers:
        print(f)

if __name__ == "__main__":
    main()

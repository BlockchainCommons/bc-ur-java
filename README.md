# Blockchain Commons UR Java library
### _by [Hiếu Phạm](https://www.github.com/jollyjoker992) and [Christopher Allen](https://www.github.com/ChristopherA)_

* <img src="https://github.com/BlockchainCommons/crypto-commons/blob/master/images/logos/crypto-commons-super-simple.png" width=16 valign="bottom">&nbsp;&nbsp; ***part of the [crypto commons](https://github.com/BlockchainCommons/crypto-commons/blob/master/README.md) technology family***

URs ("Uniform Resources") are a method for encoding structured binary data for transport in URIs and QR Codes. They are described in [BCR-2020-005](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2020-005-ur.md).

The original source written in C++ can be found [here](https://github.com/BlockchainCommons/bc-ur). This repo is a Java conversion.

## Status — Work-in-Progress

` Blockchain Commons UR Java library`  is an early Work-In-Progress.

## Usage Instructions
Encode single part
```java
byte[] bytes = hex2Bytes("916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018");
UR ur = UR.create(bytes);
String encoded = UREncoder.encode(ur); // "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch"
```

Encode multi parts
```java
byte[] bytes = hex2Bytes("916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018");
UR ur = UR.create(bytes);

// try-with-resource to make sure encoder is closed as expectation to avoid memory leak
try (UREncoder encoder = new UREncoder(ur, 30)) {
    int segNum = (int) encoder.getSeqNum();
    String[] parts = new String[segNum];
    for (int i = 0; i < segNum; i++) {
        parts[i] = encoder.nextPart();
    }
} catch (URException e) {
    // error goes here
}
```

Decode single part
```java
String encoded = "ur:bytes/hdeymejtswhhylkepmykhhtsytsnoyoyaxaedsuttydmmhhpktpmsrjtgwdpfnsboxgwlbaawzuefywkdplrsrjynbvygabwjldapfcsdwkbrkch";
UR ur = URDecoder.decode(encoded);
```

Decode multi parts
```java
// simulate encoder/decoder working together
byte[] bytes = hex2Bytes("916ec65cf77cadf55cd7f9cda1a13026ddd42e905b77adc36e4f2d3ccba44f7f4f2de44f42d84c374a0e149136f25b018");
UR ur = UR.create(bytes);

// try-with-resource to make sure encoder is closed as expectation to avoid memory leak
try (UREncoder encoder = new UREncoder(ur, 30);
    URDecoder decoder = new URDecoder()) {
    do {
        String part = encoder.nextPart();
        decoder.receivePart(part);
    } while (!decoder.isComplete());

    // received UR after decoding
    UR resultUR = decoder.resultUR();
} catch (URException e) {
    // error goes here
}
```

## Origin, Authors, Copyright & Licenses

Unless otherwise noted (either in this [/README.md](./README.md) or in the file's header comments) the contents of this repository are Copyright © 2020 by Blockchain Commons, LLC, and are [licensed](./LICENSE) under the [spdx:BSD-2-Clause Plus Patent License](https://spdx.org/licenses/BSD-2-Clause-Patent.html).

In most cases, the authors, copyright, and license for each file reside in header comments in the source code. When it does not, we have attempted to attribute it accurately in the table below.

This table below also establishes provenance (repository of origin, permalink, and commit id) for files included from repositories that are outside of this repo. Contributors to these files are listed in the commit history for each repository, first with changes found in the commit history of this repo, then in changes in the commit history of their repo of their origin.

| File      | From                                                         | Commit                                                       | Authors & Copyright (c)                                | License                                                     |
| --------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------ | ----------------------------------------------------------- |
| exception-to-the-rule.c or exception-folder | [https://github.com/community/repo-name/PERMALINK](https://github.com/community/repo-name/PERMALINK) | [https://github.com/community/repo-name/commit/COMMITHASH]() | 2020 Exception Author  | [MIT](https://spdx.org/licenses/MIT)                        |

### Derived from…

This  `Blockchain Commons UR Java library` project is either derived from or was inspired by:

- [blockchain-commons/bc-ur/](https://github.com/BlockchainCommons/bc-ur) — Repo that is UR Reference Implementation in C++, from [Blockchain-Commons](https://github.com/BlockchainCommons).


## Financial Support

`Blockchain Commons UR Java library` is a project of [Blockchain Commons](https://www.blockchaincommons.com/). We are proudly a "not-for-profit" social benefit corporation committed to open source & open development. Our work is funded entirely by donations and collaborative partnerships with people like you. Every contribution will be spent on building open tools, technologies, and techniques that sustain and advance blockchain and internet security infrastructure and promote an open web.

To financially support further development of `Blockchain Commons UR Java library` and other projects, please consider becoming a Patron of Blockchain Commons through ongoing monthly patronage as a [GitHub Sponsor](https://github.com/sponsors/BlockchainCommons). You can also support Blockchain Commons with bitcoins at our [BTCPay Server](https://btcpay.blockchaincommons.com/).

## Contributing

We encourage public contributions through issues and pull requests! Please review [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our development process. All contributions to this repository require a GPG signed [Contributor License Agreement](./CLA.md).

### Discussions

The best place to talk about Blockchain Commons and its projects is in our GitHub Discussions areas.

[**Gordian System Discussions**](https://github.com/BlockchainCommons/Gordian/discussions). For users and developers of the Gordian system, including the Gordian Server, Bitcoin Standup technology, QuickConnect, and the Gordian Wallet. If you want to talk about our linked full-node and wallet technology, suggest new additions to our Bitcoin Standup standards, or discuss the implementation our standalone wallet, the Discussions area of the [main Gordian repo](https://github.com/BlockchainCommons/Gordian) is the place.

[**Wallet Standard Discussions**](https://github.com/BlockchainCommons/AirgappedSigning/discussions). For standards and open-source developers who want to talk about wallet standards, please use the Discussions area of the [Airgapped Signing repo](https://github.com/BlockchainCommons/AirgappedSigning). This is where you can talk about projects like our [LetheKit](https://github.com/BlockchainCommons/bc-lethekit) and command line tools such as [seedtool](https://github.com/BlockchainCommons/bc-seedtool-cli), both of which are intended to testbed wallet technologies, plus the libraries that we've built to support your own deployment of wallet technology such as [bc-bip39](https://github.com/BlockchainCommons/bc-bip39), [bc-slip39](https://github.com/BlockchainCommons/bc-slip39), [bc-shamir](https://github.com/BlockchainCommons/bc-shamir), [Shamir Secret Key Recovery](https://github.com/BlockchainCommons/bc-sskr), [bc-ur](https://github.com/BlockchainCommons/bc-ur), and the [bc-crypto-base](https://github.com/BlockchainCommons/bc-crypto-base). If it's a wallet-focused technology or a more general discussion of wallet standards,discuss it here.

[**Blockchain Commons Discussions**](https://github.com/BlockchainCommons/Community/discussions). For developers, interns, and patrons of Blockchain Commons, please use the discussions area of the [Community repo](https://github.com/BlockchainCommons/Community) to talk about general Blockchain Commons issues, the intern program, or topics other than the [Gordian System](https://github.com/BlockchainCommons/Gordian/discussions) or the [wallet standards](https://github.com/BlockchainCommons/AirgappedSigning/discussions), each of which have their own discussion areas.

### Other Questions & Problems

As an open-source, open-development community, Blockchain Commons does not have the resources to provide direct support of our projects. Please consider the discussions area as a locale where you might get answers to questions. Alternatively, please use this repository's [issues](./issues) feature. Unfortunately, we can not make any promises on response time.

If your company requires support to use our projects, please feel free to contact us directly about options. We may be able to offer you a contract for support from one of our contributors, or we might be able to point you to another entity who can offer the contractual support that you need.

### Credits

The following people directly contributed to this repository. You can add your name here by getting involved. The first step is learning how to contribute from our [CONTRIBUTING.md](./CONTRIBUTING.md) documentation.

| Name              | Role                | Github                                            | Email                                 | GPG Fingerprint                                    |
| ----------------- | ------------------- | ------------------------------------------------- | ------------------------------------- | -------------------------------------------------- |
| Christopher Allen | Principal Architect | [@ChristopherA](https://github.com/ChristopherA) | \<ChristopherA@LifeWithAlacrity.com\> | FDFE 14A5 4ECB 30FC 5D22  74EF F8D3 6C91 3574 05ED |
| Hiếu Phạm | Software Engineer | [@jollyjoker992](https://github.com/jollyjoker992) | \<hieupham@bitmark.com\> | D14E 5669 6799 6C69 4B22 E13A 7E80 8AC5 0392 AD3A |

### Contributing Sponsor

`Blockchain Commons UR Java library` was produced as a collaboration between Blockchain Commons and one of our patrons, Bitmark: Bitmark wrote the library based on Blockchain Commons' specifications and reference libraries. Blockchain Commons is dedicated to not just creating open infrastructure on our own, but also coordinating the work of other companies in benefiting the Commons. Thanks to Bitmark for being the first company to work directly with us in this manner.

![](images/logos/bitmark-logo.png)

## Responsible Disclosure

We want to keep all of our software safe for everyone. If you have discovered a security vulnerability, we appreciate your help in disclosing it to us in a responsible manner. We are unfortunately not able to offer bug bounties at this time.

We do ask that you offer us good faith and use best efforts not to leak information or harm any user, their data, or our developer community. Please give us a reasonable amount of time to fix the issue before you publish it. Do not defraud our users or us in the process of discovery. We promise not to bring legal action against researchers who point out a problem provided they do their best to follow the these guidelines.

### Reporting a Vulnerability

Please report suspected security vulnerabilities in private via email to ChristopherA@BlockchainCommons.com (do not use this email for support). Please do NOT create publicly viewable issues for suspected security vulnerabilities.

The following keys may be used to communicate sensitive information to developers:

| Name              | Fingerprint                                        |
| ----------------- | -------------------------------------------------- |
| Christopher Allen | FDFE 14A5 4ECB 30FC 5D22  74EF F8D3 6C91 3574 05ED |

You can import a key by running the following command with that individual’s fingerprint: `gpg --recv-keys "<fingerprint>"` Ensure that you put quotes around fingerprints that contain spaces.

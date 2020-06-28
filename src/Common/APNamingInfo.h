// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Brno University of Technology, PRISTINE project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef APNAMINGINFO_H_
#define APNAMINGINFO_H_

//Standard libraries
#include <string>
#include <sstream>

//RINASim libraries
#include "Common/APN.h"

/**
 * @brief APNamingInfo holds complete naming info for particular application process.
 *
 * APNI contains for internal properties: APN, AP-instance id,
 * AE name, AE-instance id. Only the first one is mandatory, the rest
 * is optional.
 *
 * @authors Vladimir Vesely (ivesely@fit.vutbr.cz)
 * @date Last refactorized and documented on 2014-10-28
 */
class APNamingInfo
{
  public:
    /**
     * @brief Constructor of blank APNI
     */
    APNamingInfo() = default;

    /**
     * @brief Constructor of APNI with only APN initialized
     * @param name Name of APN
     */
    APNamingInfo(const std::string &name);

    /**
     * @brief Constructor of APNI with only APN initialized
     * @param apn New APN
     */
    APNamingInfo(const APN &apn);

    /**
     * @brief Construcor of fully initialized APNI
     * @param apn New APN
     * @param apinstance New AP instance identifier
     * @param aename New AE identifier
     * @param aeinstance New AE instance identifier
     */
    APNamingInfo(const APN &apn,
                 const std::string &apinstance,
                 const std::string &aename,
                 const std::string &aeinstance);

    /**
     * @brief Destructor
     */
    ~APNamingInfo() = default;

    /**
     * @brief Equal operator overload
     * @param other Other APNI to which this one is being compared
     * @return Returns true if all APN, AP-instance id, AE name and AE-instance id
     *         are equl. Otherwise returns false.
     */
    bool operator==(const APNamingInfo &other) const
    {
        return (apn == other.apn && apinstance == other.apinstance &&
                aename == other.aename && aeinstance == other.aeinstance);
    }

    /**
     * @brief Text output suitable for << string streams and WATCH
     * @return APNI string representation
     */
    [[nodiscard]] std::string str() const;

    /**
     * @brief Getter of AE-instance attribute
     * @return AE-instance id value
     */
    [[nodiscard]] const std::string &getAeinstance() const { return aeinstance; }

    /**
     * @brief Setter of AE-instance attribute
     * @param aeinstance A new AE-instance id value
     */
    void setAeinstance(const std::string& aeinstance) {
        this->aeinstance = aeinstance;
    }

    /**
     * @brief Getter of AE name
     * @return AE name value
     */
    [[nodiscard]] const std::string &getAename() const { return aename; }

    /**
     * @brief Setter of AE name attribute
     * @param aename A new AE name value
     */
    void setAename(const std::string &aename) { this->aename = aename; }

    /**
     * @brief Getter of AP-instance id
     * @return AP-instance id value
     */
    [[nodiscard]] const std::string &getApinstance() const { return apinstance; }

    /**
     * @brief Setter of AP-instance id
     * @param apinstance A new AP-instance id value
     */
    void setApinstance(const std::string &apinstance) { this->apinstance = apinstance; }

    /**
     * @brief Getter of APN
     * @return APN
     */
    [[nodiscard]] const APN &getApn() const { return apn; }

    /**
     * @brief Setter of APN
     * @param apn A new APN value
     */
    void setApn(const APN& apn) {
        this->apn = apn;
    }

  protected:
    /**
     * @brief Mandatory APN
     */
    APN apn;

    /**
     * @brief Optional AP-instance id
     */
    std::string apinstance;

    /**
     * @brief Optional AE name
     */
    std::string aename;

    /**
     * @brief Optional AE-instance id
     */
    std::string aeinstance;
};

/**
 * @brief APNamingInfo is subclassed by APNI for purely estetic purposes
 */
class APNI: public APNamingInfo {};

//Free function
/**
 * @brief << operator overload that calls APNI.str() method
 * @param os Resulting ostream
 * @param apni APNI class that is being converted to string
 * @return Infotext representing APNI
 */
std::ostream& operator<< (std::ostream& os, const APNamingInfo& apni);

class APNIPair : public cObject
{
  public:
    APNIPair();
    APNIPair(const APNamingInfo &src, const APNamingInfo &dst);
    APNIPair(const std::string &src, const std::string &dst);
    ~APNIPair() override = default;

    [[nodiscard]] std::string str() const override;

    APNamingInfo first;
    APNamingInfo second;
};

std::ostream& operator<< (std::ostream& os, const APNIPair& apnip);

#endif /* APNAMINGINFO_H_ */

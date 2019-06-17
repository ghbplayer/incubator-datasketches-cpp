/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _HLL6ARRAY_HPP_
#define _HLL6ARRAY_HPP_

#include "HllArray.hpp"
#include "HllPairIterator.hpp"

namespace datasketches {

class Hll6Array final : public HllArray {
  public:
    explicit Hll6Array(int lgConfigK);
    explicit Hll6Array(const Hll6Array& that);

    virtual ~Hll6Array();

    virtual Hll6Array* copy() const;

    virtual std::unique_ptr<PairIterator> getIterator() const;

    virtual int getSlot(int slotNo) const final;
    virtual void putSlot(int slotNo, int value) final;

    virtual HllSketchImpl* couponUpdate(int coupon) final;

    virtual int getHllByteArrBytes() const;

  protected:
    friend class Hll6Iterator;
};

class Hll6Iterator : public HllPairIterator {
  public:
    Hll6Iterator(const Hll6Array& array, int lengthPairs);
    virtual int value();

    virtual ~Hll6Iterator();

  private:
    const Hll6Array& hllArray;
    int bitOffset;
};

}

#endif /* _HLL6ARRAY_HPP_ */

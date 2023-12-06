// "use client";

import { useState } from "react";
import { Button } from "@nextui-org/react";

export default function CardSdCardTest() {
  const [sdCardInfo, setSdCardInfo] = useState(
    "Click the button to get SD card info"
  );

  function handleUpdateSdCardInfo() {
    console.log("handle update sd card info");

    setSdCardInfo("Updating...");

    fetch("/api/v1/check_sdcard")
      .then((response) => response.json())
      .then((data) => {
        console.log(data.info);
        setSdCardInfo(data.info);
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
        setSdCardInfo("😓 Failed :(");
      });
  }

  return (
    <div>
      <div className="mx-5 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold">SD Card</p>

        <Button
          radius="full"
          variant="flat"
          color={sdCardInfo === "Updating..." ? "warning" : "success"}
          onPress={() => handleUpdateSdCardInfo()}
          isLoading={sdCardInfo === "Updating..."}
        >
          {sdCardInfo === "Updating..." ? "" : "Update"}
        </Button>
      </div>

      <div className="mx-5 mb-5 flex gap-5">
        <p>{sdCardInfo}</p>
      </div>
    </div>
  );
}

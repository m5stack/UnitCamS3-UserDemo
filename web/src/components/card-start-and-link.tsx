// "use client";

import { Card, Link } from "@nextui-org/react";
import { useState, useEffect } from "react";
import { QRCodeSVG } from "qrcode.react";

export default function CardStartAndLink() {
  const [imageLink, setImageLink] = useState("");
  //   const [imageLink, setImageLink] = useState(
  //     "https://ezdata-dev-bucket.oss-cn-shenzhen.aliyuncs.com/asdasdasdasdasd/asdasdasdasd/asdasdasda"
  //   );

  function handleStartPoster() {
    console.log("start poster");
    fetch("/api/v1/start_poster")
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
      });
  }

  function handleUpdateImageLink(mac: string) {
    // Remove ":"
    let newMac = mac.replaceAll(":", "");
    console.log("get mac:", newMac);

    // Merge link
    let newLink =
      "https://ezdata2.m5stack.com/" +
      newMac +
      "/captured.jpg";
    console.log("new link:", newLink);

    setImageLink(newLink);
    handleStartPoster();
  }

  useEffect(() => {
    // Get mac
    fetch("/api/v1/get_mac")
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
        handleUpdateImageLink(data.mac);
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
      });
  }, []);

  return (
    <Card className="grow">
      <div className="flex flex-col">
        <QRCodeSVG
          className="mt-10 self-center"
          value={imageLink}
          size={256}
          bgColor={"#ffffff"}
          fgColor={"#000000"}
          level={"L"}
          includeMargin={false}
        />

        <Link
          isExternal
          href={imageLink}
          showAnchorIcon
          className="mt-5 self-center"
        >
          Image Link
        </Link>

        <p className="mx-5 my-5 self-center text-center break-all">
          {imageLink}
        </p>
      </div>
    </Card>
  );
}

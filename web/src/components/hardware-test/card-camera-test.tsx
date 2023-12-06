// "use client";

import { useState } from "react";
import { Button, Image, Link } from "@nextui-org/react";
// import Link from "next/link";

export default function CardCameraTest() {
  const [camCapturePath, setCamCapturePath] = useState("");

  function handleCamCapture() {
    console.log("handle cam capture");
    setCamCapturePath("/api/v1/capture?" + new Date().getTime().toString());
  }

  //   function handleCamStream() {
  //     console.log("handle cam stream");
  //     setCamCapturePath("/stream");
  //   }

  return (
    <div className="flex flex-col items-center">
      <p className="self-start mx-5 mt-5 text-3xl font-serif font-bold">
        Camera
      </p>

      <div className="mx-10 mt-5 mb-5">
        <Image
          className="shadow-lg"
          alt="Captured Image"
          //   src="https://nextui-docs-v2.vercel.app/images/hero-card-complete.jpeg"
          src={camCapturePath}
        />
      </div>

      <div className="mx-10 mb-5 flex gap-x-5">
        <Button
          radius="full"
          // className="bg-gradient-to-tr from-pink-500 to-yellow-500 text-white shadow-lg"
          variant="flat"
          color="success"
          onPress={() => handleCamCapture()}
        >
          Capture
        </Button>

        <Link href="/api/v1/stream">
          <Button
            radius="full"
            // className="bg-gradient-to-tr from-pink-500 to-yellow-500 text-white shadow-lg"
            variant="flat"
            color="warning"
            // onPress={() => handleCamStream()}
          >
            Stream
          </Button>
        </Link>
      </div>
    </div>
  );
}

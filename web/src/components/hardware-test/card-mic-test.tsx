// "use client";

import { Button, Link } from "@nextui-org/react";
import { useState } from "react";

type MicState = "waitFirst" | "recording" | "waitNext";

export default function CardMicTest() {
  let nIntervId: number;
  const [micState, setMicState] = useState("waitFirst" as MicState);

  function handleButtonRecord() {
    console.log("start recording");

    // Start recording
    fetch("/api/v1/mic_start");

    setMicState("recording");
    nIntervId = setInterval(handleCheckIsRecording, 2000);
  }

  function handleCheckIsRecording() {
    console.log("check record state");

    fetch("/api/v1/mic_is_recording")
      .then((response) => response.json())
      .then((data) => {
        console.log(data.msg);
        // Done recording
        if (data.msg === "no") {
          clearInterval(nIntervId);
          setMicState("waitNext");
        }
      });
  }

  return (
    <div>
      <div className="mx-5 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold">Mic</p>

        <Button
          radius="full"
          variant="flat"
          color={micState === "recording" ? "warning" : "success"}
          onPress={() => handleButtonRecord()}
          isLoading={micState === "recording"}
        >
          {micState === "recording" ? "" : "Record 4s"}
        </Button>
      </div>

      <div className="mx-5 mb-5">
        {micState === "waitFirst" && <p>Click the button to record audio</p>}

        {micState === "recording" && (
          <div className="flex gap-5">
            <p>Recording, please wait...</p>
            {/* <Spinner size="sm" color="warning"></Spinner> */}
          </div>
        )}

        {micState === "waitNext" && (
          <div className="flex gap-5">
            <audio controls>
              <source src="/wav/rec.wav" type="audio/wav"></source>
              Your browser does not support the audio element.
            </audio>
            <Link href="/wav/rec.wav">
              <Button radius="full" variant="flat" color="success">
                Download
              </Button>
            </Link>
          </div>
        )}
      </div>
    </div>
  );
}

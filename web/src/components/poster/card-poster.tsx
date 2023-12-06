// "use client";

import { Button } from "@nextui-org/react";
import { useNavigate } from "react-router-dom";

export default function CardPoster() {
  const navigate = useNavigate();

  return (
    <div>
      <div className="mx-5 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold">Image Poster</p>
        <Button
          radius="full"
          variant="flat"
          color="success"
          onPress={() => navigate("/poster")}
        >
          Setup
        </Button>
      </div>

      <div className="mx-5 mb-5">
        <p>
          Posting captured image to
          <span className="font-bold"> EzData </span>
          regularly
        </p>
      </div>
    </div>
  );
}

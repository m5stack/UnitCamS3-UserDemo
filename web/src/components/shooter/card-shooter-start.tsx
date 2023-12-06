// "use client";

import { useEffect } from "react";
import CardShooterNotice from "../../components/shooter/card-shooter-notice";

export default function CardShooterStart() {
  function handleStartShooter() {
    console.log("start shooter");
    fetch("/api/v1/start_shooter")
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
  useEffect(() => {
    handleStartShooter();
  }, []);

  return <CardShooterNotice></CardShooterNotice>;
}

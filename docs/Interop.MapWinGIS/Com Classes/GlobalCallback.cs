using System;

#if nsp
namespace MapWinGIS
{
#endif

    /// <summary>
    /// An interface for callback objects which can be used to return information about progress and errors.     
    /// </summary>
    /// <remarks>Here is an example which demonstrates the usage of Callback.</remarks>
    /// \code
    /// public void Test()
    /// {
    ///     Shapefile sf = new Shapefile();
    ///     sf.GlobalCallback = new Callback();
    /// }
    /// class Callback : ICallback
    /// { 
    ///     public void Error(string KeyOfSender, string ErrorMsg)
    ///     {
    ///         Debug.Print("Error reported: " + ErrorMsg);
    ///     }
    ///     public void Progress(string KeyOfSender, int Percent, string Message)
    ///     {
    ///         Debug.Print(Message + ": " + Percent + "%");
    ///     }
    /// }
    /// \endcode
    #if nsp
        public interface ICallback
    #else
        public interface ICallback
    #endif
{
    /// <summary>
    /// A function to report the information about an error.
    /// </summary>
    /// <param name="KeyOfSender">The key associated with the object in which error took place.</param>
    /// <param name="ErrorMsg">The description of the error.</param>
    void Error(string KeyOfSender, string ErrorMsg);
    

    /// <summary>
    /// A function to report the information about progress of the task.
    /// </summary>
    /// <param name="KeyOfSender">The key associated with the object in which error took place.</param>
    /// <param name="Percent">The percent of completion (from 0 to 100).</param>
    /// <param name="Message">The information about the task being executed.</param>
    void Progress(string KeyOfSender, int Percent, string Message);
}
#if nsp
}
#endif
// ReSharper disable CheckNamespace
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
    ///     public void Error(string keyOfSender, string errorMsg)
    ///     {
    ///         Debug.Print("Error reported: " + errorMsg);
    ///     }
    ///     public void Progress(string keyOfSender, int percent, string message)
    ///     {
    ///         Debug.Print(message + ": " + percent + "%");
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
    /// <param name="keyOfSender">The key associated with the object in which error took place.</param>
    /// <param name="errorMsg">The description of the error.</param>
    void Error(string keyOfSender, string errorMsg);
    

    /// <summary>
    /// A function to report the information about progress of the task.
    /// </summary>
    /// <param name="keyOfSender">The key associated with the object in which error took place.</param>
    /// <param name="percent">The percent of completion (from 0 to 100).</param>
    /// <param name="message">The information about the task being executed.</param>
    void Progress(string keyOfSender, int percent, string message);
}
#if nsp
}
#endif